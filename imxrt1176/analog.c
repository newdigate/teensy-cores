/* RT1176 LPADC driver: blocking analogRead + resolution.
 * Fresh implementation against the LPADC command/trigger/RESFIFO model (the
 * teensy4 ADC driver targets the different RT106x ADC and does not port).
 * No calibration: PERI_ADC.h exposes no CTRL.CAL bit and QEMU does not model it;
 * uncalibrated single-shot reads are adequate for this bring-up.
 *
 * This file is released into the public domain. */
#include "core_pins.h"
#include "imxrt1176.h"
#include "pins_arduino.h"

/* The verified analog pin table (declared extern in pins_arduino.h). */
const analog_pin_info_t analog_pin_to_info[NUM_ANALOG_INPUTS] = {
    { A0, 0, 0, 0 },   /* A0 -> LPADC1, channel 0, side A */
};

typedef struct {
    volatile uint32_t *CTRL, *STAT, *IE, *CFG, *FCTRL, *SWTRIG, *TCTRL0, *CMDL1, *CMDH1, *RESFIFO;
    volatile uint32_t *clock_root, *lpcg;
    IRQ_NUMBER_t irq;   /* IE/FCTRL/irq are used by the async path (analogReadAsync) */
} lpadc_regs_t;

static const lpadc_regs_t LPADC[2] = {
    { &ADC1_CTRL,&ADC1_STAT,&ADC1_IE,&ADC1_CFG,&ADC1_FCTRL,&ADC1_SWTRIG,&ADC1_TCTRL0,&ADC1_CMDL1,&ADC1_CMDH1,&ADC1_RESFIFO,
      &CCM_CLOCK_ROOT9_CONTROL,&CCM_LPCG55_DIRECT, IRQ_ADC1 },
    { &ADC2_CTRL,&ADC2_STAT,&ADC2_IE,&ADC2_CFG,&ADC2_FCTRL,&ADC2_SWTRIG,&ADC2_TCTRL0,&ADC2_CMDL1,&ADC2_CMDH1,&ADC2_RESFIFO,
      &CCM_CLOCK_ROOT10_CONTROL,&CCM_LPCG56_DIRECT, IRQ_ADC2 },
};

static uint8_t  lpadc_inited[2] = {0,0};
static uint8_t  read_res_bits = 10;   /* Arduino/Teensy default */

static void lpadc_init(int i) {
    const lpadc_regs_t *a = &LPADC[i];
    if (lpadc_inited[i]) return;
    *a->clock_root = 0;                                  /* OscRC48MDiv2 (mux 0), div 1 */
    *a->lpcg = 1u;                                       /* enable LPCG gate */
    *a->CTRL = ADC_CTRL_RST;                             /* reset */
    *a->CTRL = 0u;
    *a->CFG = ADC_CFG_PWRSEL(3) | ADC_CFG_REFSEL(1);     /* high power, Vref Alt1 */
    *a->CTRL = ADC_CTRL_ADCEN;                           /* enable */
    lpadc_inited[i] = 1;
}

static uint16_t scale12(uint16_t d) {
    if (read_res_bits <= 12) return (uint16_t)(d >> (12 - read_res_bits));
    return (uint16_t)(d << (read_res_bits - 12));
}

uint16_t analogReadChannel(uint8_t instance, uint8_t channel) {
    if (instance > 1) return 0;
    const lpadc_regs_t *a = &LPADC[instance];
    lpadc_init(instance);
    *a->CMDL1  = ADC_CMDL_ADCH(channel);   /* command buffer 1: channel */
    *a->CMDH1  = 0u;                        /* single sample, no average/loop */
    *a->TCTRL0 = ADC_TCTRL_TCMD(1);        /* trigger 0 -> command 1 */
    *a->SWTRIG = 1u;                        /* software-trigger 0 */
    uint32_t guard = 100000u;
    while (!((*a->STAT) & ADC_STAT_RDY) && --guard) { }  /* bounded (never hangs) */
    /* On timeout the VALID check below returns 0; a late result could linger in
     * the FIFO, but this path is unreachable at ~996 MHz vs LPADC conversion time. */
    uint32_t r = *a->RESFIFO;              /* pops the FIFO entry */
    if (!(r & ADC_RESFIFO_VALID)) return 0;
    return scale12((uint16_t)(r & ADC_RESFIFO_D));
}

int analogRead(uint8_t pin) {
    for (uint8_t k = 0; k < NUM_ANALOG_INPUTS; k++) {
        if (analog_pin_to_info[k].pin == pin)
            return analogReadChannel(analog_pin_to_info[k].instance, analog_pin_to_info[k].channel);
    }
    return 0;   /* not an analog pin */
}

/* Defines the extern declared in core_pins.h; analogReadResolution() is an inline
 * wrapper there that calls this. */
void analogReadRes(unsigned int bits) {
    if (bits < 1u) bits = 1u;
    if (bits > 16u) bits = 16u;
    read_res_bits = (uint8_t)bits;
}

void analogReference(uint8_t type) { (void)type; }  /* stub: Vref fixed to Alt1 in init */

/* Declared in core_pins.h; stubbed so sketches that call it link. Hardware
 * averaging (CMDH.AVGS) is not wired yet. */
void analogReadAveraging(unsigned int num) { (void)num; }

/* ---- Interrupt-driven async conversion path (analogReadAsync) -------------
 * Sets IE.FWMIE with FWMARK=0 so the first FIFO result raises the LPADC IRQ;
 * the ISR pops RESFIFO, scales it, disables the interrupt, and delivers the
 * value via the registered callback. One pending conversion per instance. */
static void (* volatile async_cb[2])(uint16_t) = {0, 0};
static volatile uint8_t async_pending[2] = {0, 0};

static void lpadc_isr(int i) {
    const lpadc_regs_t *a = &LPADC[i];
    *a->IE = 0u;                            /* disable FIFO-watermark interrupt */
    uint32_t r = *a->RESFIFO;               /* pop result */
    uint8_t bits = read_res_bits;           /* read once (ISR-safety: avoid mid-change) */
    uint16_t raw = (uint16_t)(r & ADC_RESFIFO_D);
    uint16_t v = (r & ADC_RESFIFO_VALID) ? ((bits <= 12) ? (uint16_t)(raw >> (12 - bits))
                                                          : (uint16_t)(raw << (bits - 12)))
                                         : 0;
    async_pending[i] = 0;
    if (async_cb[i]) async_cb[i](v);
}
static void lpadc1_isr(void) { lpadc_isr(0); }
static void lpadc2_isr(void) { lpadc_isr(1); }

/* Shared start routine for a raw (instance,channel) async conversion. */
static int lpadc_start_async(int i, uint8_t channel, void (*callback)(uint16_t)) {
    if (i < 0 || i > 1) return 0;
    if (async_pending[i]) return 0;         /* one conversion per instance */
    lpadc_init(i);
    const lpadc_regs_t *a = &LPADC[i];
    async_cb[i] = callback;
    async_pending[i] = 1;
    *a->FCTRL = ADC_FCTRL_FWMARK(0);
    *a->IE = ADC_IE_FWMIE;
    attachInterruptVector(a->irq, (i == 0) ? &lpadc1_isr : &lpadc2_isr);
    NVIC_ENABLE_IRQ(a->irq);
    *a->CMDL1  = ADC_CMDL_ADCH(channel);
    *a->CMDH1  = 0u;
    *a->TCTRL0 = ADC_TCTRL_TCMD(1);
    *a->SWTRIG = 1u;                        /* fires; ISR delivers the result */
    return 1;
}

int analogReadAsync(uint8_t pin, void (*callback)(uint16_t value)) {
    for (uint8_t k = 0; k < NUM_ANALOG_INPUTS; k++) {
        if (analog_pin_to_info[k].pin == pin)
            return lpadc_start_async(analog_pin_to_info[k].instance,
                                     analog_pin_to_info[k].channel, callback);
    }
    return 0;   /* not an analog pin */
}

/* Test hook: start an async conversion on a raw (instance,channel). */
int analogReadAsyncChannel(uint8_t instance, uint8_t channel, void (*callback)(uint16_t value)) {
    return lpadc_start_async((int)instance, channel, callback);
}
