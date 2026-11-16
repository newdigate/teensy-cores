#include "IntervalTimer.h"

#define PIT_NUM_CH 4

static void (*volatile pit_callback[PIT_NUM_CH])(void);
static uint8_t pit_priority[PIT_NUM_CH];
static bool    pit_used[PIT_NUM_CH];
static bool    pit_isr_installed = false;

/* Effective PIT1 input clock = BUS root (CLOCK_ROOT2), derived at runtime so it
 * tracks whatever startup/clocks program. Mirror of imxrt_ccm's bus_root_clk.
 *
 * The mux table is the RM's BUS clock root (fsl_clock.h kCLOCK_BUS_ClockRoot_Mux*).
 * Until 2026-11-16 only mux 0 was decoded and every other source was taken as
 * 24 MHz; startup.c routes BUS to SYS_PLL3/2 = 240 MHz, so on silicon the PIT ran
 * 20x faster than every IntervalTimer/tone() period asked for (Nosh bench:
 * tone(1000) measured 20 kHz). QEMU's imxrt_ccm had the same gap, so the gates
 * agreed with the firmware while both were wrong by the same factor. */
static uint32_t pit_clock_hz(void)
{
    uint32_t ctrl = CCM_CLOCK_ROOT2_CONTROL;
    uint32_t mux  = (ctrl >> 8) & 0x7u;
    uint32_t div  = (ctrl & 0xFFu) + 1u;
    uint32_t src;
    switch (mux) {
    case 0:  src = 24000000u;  break;  /* OSC_RC_48M / 2 */
    case 1:  src = 24000000u;  break;  /* OSC_24M (crystal) */
    case 2:  src = 400000000u; break;  /* OSC_RC_400M */
    case 3:  src = 16000000u;  break;  /* OSC_RC_16M */
    case 4:  src = 480000000u; break;  /* SYS_PLL3_OUT (startup.c's choice -> /2 = 240 MHz) */
    case 5:  src = 200000000u; break;  /* SYS_PLL1_DIV5 (1 GHz / 5) */
    case 6:  src = 528000000u; break;  /* SYS_PLL2_OUT */
    default: {                         /* 7: SYS_PLL2_PFD3 = 528 MHz * 18 / FRAC */
        uint32_t frac = (*(volatile uint32_t *)0x40C84270u >> 24) & 0x3Fu; /* ANADIG SYS_PLL2_PFD, PFD3_FRAC */
        if (frac < 12u) frac = 12u;    /* RM: FRAC 12..35 */
        src = (uint32_t)(528000000ull * 18u / frac);
        break;
    }
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
