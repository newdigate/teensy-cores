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

/* The verified analog pin table (declared extern in pins_arduino.h).
 * Channel/side per RT1170 RM "ADC external signals" (ADC1_CHnA/B are dedicated
 * pad functions, no muxing) cross-checked against the RevC3 schematic sheet 26
 * (J26 socket nets, all via populated 0R) -- see docs/arduino-header-revc3.md.
 * NOTE the header order is NOT channel order: A4/A5 sit on the LOWER channels. */
const analog_pin_info_t analog_pin_to_info[NUM_ANALOG_INPUTS] = {
    { A0, 0, 2, 0 },   /* A0 -> GPIO_AD_10 = ADC1 CH2 side A */
    { A1, 0, 2, 1 },   /* A1 -> GPIO_AD_11 = ADC1 CH2 side B */
    { A2, 0, 3, 0 },   /* A2 -> GPIO_AD_12 = ADC1 CH3 side A */
    { A3, 0, 3, 1 },   /* A3 -> GPIO_AD_13 = ADC1 CH3 side B */
    { A4, 0, 1, 1 },   /* A4 -> GPIO_AD_09 = ADC1 CH1 side B (also LPI2C1 SDA) */
    { A5, 0, 1, 0 },   /* A5 -> GPIO_AD_08 = ADC1 CH1 side A (also LPI2C1 SCL) */
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

/* Scale a RESFIFO sample to `bits` resolution. A standard-resolution 12-bit
 * single-ended result occupies RESFIFO.D[14:3] -- bit 15 is the differential
 * sign bit, NOT data (SDK lpadc examples: convValue >> 3; full range 4096).
 * So full scale is 0x7FF8 -> right-shift by (15 - bits). (bits is clamped to
 * 1..16 by analogReadRes.) HW-verified 2026-07-13 via the DAC->ADC1_CH6A
 * loopback: the earlier D[15:4] assumption halved every reading. The ISR
 * passes a snapshot of read_res_bits (ISR-safety); scale_res() uses the live
 * value. Single source of truth. */
static uint16_t scale_res_bits(uint16_t d, uint8_t bits) {
    if (bits >= 15) return (uint16_t)(d << (bits - 15));
    return (uint16_t)(d >> (15 - bits));
}
static uint16_t scale_res(uint16_t d) { return scale_res_bits(d, read_res_bits); }

static uint16_t lpadc_read(uint8_t instance, uint8_t channel, uint8_t side) {
    if (instance > 1) return 0;
    const lpadc_regs_t *a = &LPADC[instance];
    lpadc_init(instance);
    *a->CMDL1  = ADC_CMDL_ADCH(channel)    /* command buffer 1: channel */
               | (side ? ADC_CMDL_ABSEL : 0u)  /* B-side input select */
               | ADC_CMDL_CSCALE;           /* full-scale input (CSCALE=0 attenuates 30/64) */
    *a->CMDH1  = 0u;                        /* single sample, no average/loop */
    *a->TCTRL0 = ADC_TCTRL_TCMD(1);        /* trigger 0 -> command 1 */
    *a->SWTRIG = 1u;                        /* software-trigger 0 */
    uint32_t guard = 100000u;
    while (!((*a->STAT) & ADC_STAT_RDY) && --guard) { }  /* bounded (never hangs) */
    /* On timeout the VALID check below returns 0; a late result could linger in
     * the FIFO, but this path is unreachable at ~996 MHz vs LPADC conversion time. */
    uint32_t r = *a->RESFIFO;              /* pops the FIFO entry */
    if (!(r & ADC_RESFIFO_VALID)) return 0;
    return scale_res((uint16_t)(r & ADC_RESFIFO_D));
}

/* Test hook / raw access: A-side single-ended conversion. */
uint16_t analogReadChannel(uint8_t instance, uint8_t channel) {
    return lpadc_read(instance, channel, 0);
}

int analogRead(uint8_t pin) {
    for (uint8_t k = 0; k < NUM_ANALOG_INPUTS; k++) {
        if (analog_pin_to_info[k].pin == pin)
            return lpadc_read(analog_pin_to_info[k].instance,
                              analog_pin_to_info[k].channel,
                              analog_pin_to_info[k].side);
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
    uint16_t v = (r & ADC_RESFIFO_VALID) ? scale_res_bits(raw, bits) : 0;
    async_pending[i] = 0;                    /* cleared before callback: a callback may chain another conversion */
    if (async_cb[i]) async_cb[i](v);
}
static void lpadc1_isr(void) { lpadc_isr(0); }
static void lpadc2_isr(void) { lpadc_isr(1); }

/* Shared start routine for a raw (instance,channel,side) async conversion. */
static int lpadc_start_async(int i, uint8_t channel, uint8_t side, void (*callback)(uint16_t)) {
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
    *a->CMDL1  = ADC_CMDL_ADCH(channel) | (side ? ADC_CMDL_ABSEL : 0u) | ADC_CMDL_CSCALE;
    *a->CMDH1  = 0u;
    *a->TCTRL0 = ADC_TCTRL_TCMD(1);
    *a->SWTRIG = 1u;                        /* fires; ISR delivers the result */
    return 1;
}

int analogReadAsync(uint8_t pin, void (*callback)(uint16_t value)) {
    for (uint8_t k = 0; k < NUM_ANALOG_INPUTS; k++) {
        if (analog_pin_to_info[k].pin == pin)
            return lpadc_start_async(analog_pin_to_info[k].instance,
                                     analog_pin_to_info[k].channel,
                                     analog_pin_to_info[k].side, callback);
    }
    return 0;   /* not an analog pin */
}

/* Test hook: start an async conversion on a raw (instance,channel), A side. */
int analogReadAsyncChannel(uint8_t instance, uint8_t channel, void (*callback)(uint16_t value)) {
    return lpadc_start_async((int)instance, channel, 0, callback);
}
