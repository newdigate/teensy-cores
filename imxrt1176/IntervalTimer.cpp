#include "IntervalTimer.h"

#define PIT_NUM_CH 4

static void (*volatile pit_callback[PIT_NUM_CH])(void);
static uint8_t pit_priority[PIT_NUM_CH];
static bool    pit_used[PIT_NUM_CH];
static bool    pit_isr_installed = false;

/* Effective PIT1 input clock = BUS root (CLOCK_ROOT2), derived at runtime so it
 * tracks whatever startup/clocks program. Mirror of imxrt_ccm's bus_root_clk. */
static uint32_t pit_clock_hz(void)
{
    uint32_t ctrl = CCM_CLOCK_ROOT2_CONTROL;
    uint32_t mux  = (ctrl >> 8) & 0x7u;
    uint32_t div  = (ctrl & 0xFFu) + 1u;
    uint32_t src;
    switch (mux) {
    case 0:  src = 24000000u; break;   /* OscRC48MDiv2 (BUS mux 0) */
    default: src = 24000000u; break;   /* only mux 0 modelled; extend when BUS is re-routed */
    }
    return src / div;
}

static void pit_isr(void)
{
    for (int ch = 0; ch < PIT_NUM_CH; ch++) {
        if (PIT1_CHANNEL[ch].TFLG & PIT_TFLG_TIF) {
            PIT1_CHANNEL[ch].TFLG = PIT_TFLG_TIF;      /* W1C before callback */
            if (pit_callback[ch]) pit_callback[ch]();
        }
    }
}

static void pit_apply_priority(void)
{
    uint8_t top = 255;
    for (int ch = 0; ch < PIT_NUM_CH; ch++)
        if (pit_used[ch] && pit_priority[ch] < top) top = pit_priority[ch];
    NVIC_SET_PRIORITY(IRQ_PIT1, top);
}

uint32_t IntervalTimer::ldvalFromMicros(double us)
{
    if (us <= 0.0) return 0;
    double count = us * (double)pit_clock_hz() / 1000000.0;   /* input clocks per period */
    if (count > 4294967295.0) return 0;                       /* must fit 32-bit LDVAL */
    uint32_t ticks = (uint32_t)(count + 0.5);                 /* round to nearest */
    if (ticks < 18u) return 0;                                /* Teensy min (LDVAL >= 17) */
    return ticks - 1u;
}

bool IntervalTimer::beginPeriod(void (*funct)(void), double us)
{
    if (funct == 0) return false;
    uint32_t ldval = ldvalFromMicros(us);
    if (ldval == 0) return false;

    if (channel < 0) {                                        /* allocate a free channel */
        for (int i = 0; i < PIT_NUM_CH; i++) {
            if (!pit_used[i]) { channel = i; pit_used[i] = true; break; }
        }
        if (channel < 0) return false;                        /* all 4 busy */
    }

    CCM_LPCG62_DIRECT = 1u;                                   /* gate PIT1 clock on */
    PIT1_MCR &= ~PIT1_MCR_MDIS;                               /* enable module */
    if (!pit_isr_installed) {
        attachInterruptVector(IRQ_PIT1, pit_isr);
        NVIC_ENABLE_IRQ(IRQ_PIT1);
        pit_isr_installed = true;
    }

    pit_callback[channel] = funct;
    pit_priority[channel] = nvic_priority;
    PIT1_CHANNEL[channel].TCTRL = 0;                          /* stop while configuring */
    PIT1_CHANNEL[channel].LDVAL = ldval;
    PIT1_CHANNEL[channel].TFLG  = PIT_TFLG_TIF;               /* clear stale flag */
    PIT1_CHANNEL[channel].TCTRL = PIT_TCTRL_TEN | PIT_TCTRL_TIE;
    pit_apply_priority();
    return true;
}

void IntervalTimer::updatePeriod(double us)
{
    if (channel < 0) return;
    uint32_t ldval = ldvalFromMicros(us);
    if (ldval == 0) return;
    PIT1_CHANNEL[channel].LDVAL = ldval;                     /* effective next reload */
}

void IntervalTimer::end()
{
    if (channel < 0) return;
    PIT1_CHANNEL[channel].TCTRL = 0;                         /* stop + disable IRQ */
    PIT1_CHANNEL[channel].TFLG  = PIT_TFLG_TIF;
    pit_callback[channel] = 0;
    pit_used[channel] = false;
    channel = -1;
    pit_apply_priority();
}

void IntervalTimer::priority(uint8_t n)
{
    nvic_priority = n;
    if (channel >= 0) { pit_priority[channel] = n; pit_apply_priority(); }
}
