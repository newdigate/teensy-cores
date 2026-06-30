/* Teensyduino Core Library — MIMXRT1176-EVKB Phase-0 timing primitives.
 *
 * Provides millis(), micros(), delay(), and delayMicroseconds() for the
 * NXP i.MX RT1176 (Cortex-M7) port.  SysTick is configured for 1 kHz by
 * systick_init() in startup.c; DWT CYCCNT is enabled there too.
 *
 * systick_millis_count: strong (non-weak) definition here overrides the weak
 * placeholder in startup.c so the linker resolves a single symbol in DTCM.
 */

#include <stdint.h>
#include "imxrt1176.h"

/* -----------------------------------------------------------------------
 * Millisecond counter — incremented by systick_isr() in startup.c.
 * The STRONG definition here wins over the __attribute__((weak)) placeholder
 * in startup.c; there must be exactly one copy in the final binary.
 * --------------------------------------------------------------------- */
volatile uint32_t systick_millis_count = 0;

/* Variables referenced by core_pins.h inlines (delayMicroseconds,
 * delayNanoseconds).  F_CPU_ACTUAL tracks the live core clock; it is set to
 * 996 MHz here and may be updated by set_arm_clock_rt1176() in startup.c if
 * clock reconfiguration is performed at runtime. */
volatile uint32_t F_CPU_ACTUAL  = 996000000u;
volatile uint32_t F_BUS_ACTUAL  = 996000000u;   /* placeholder; bus != core on RT1176 */
volatile uint32_t scale_cpu_cycles_to_microseconds = 0; /* not used by Phase-0 micros() */

/* -----------------------------------------------------------------------
 * millis() — returns the number of milliseconds since boot.
 * Defined in core_pins.h as a static inline that reads systick_millis_count,
 * so no out-of-line body is needed here.  Provided as a non-inline fallback
 * for translation units that include only this header or link against the
 * compiled object directly.
 * --------------------------------------------------------------------- */
uint32_t millis(void)
{
    return systick_millis_count;
}

/* -----------------------------------------------------------------------
 * micros() — microseconds since boot, composed from the 1 kHz SysTick
 * millisecond counter and the SysTick current-value register (SYST_CVR).
 *
 * Algorithm (Teensy idiom):
 *   1. Disable interrupts, snapshot CVR and millis counter, check if a
 *      SysTick interrupt is pending (i.e. the counter wrapped to 0 and
 *      the ISR hasn't run yet), then re-enable interrupts.
 *   2. If the pending bit is set and CVR is not very close to zero (which
 *      would mean the ISR is about to run), bump ms by 1 to account for
 *      the unserviced tick.
 *   3. Ticks elapsed = (reload_value - CVR), where reload = 996000-1.
 *      Divide by 996 to convert to microseconds (996 ticks/us @ 996 MHz).
 *
 * This gives ~1 µs resolution with no 64-bit multiply in the fast path.
 * --------------------------------------------------------------------- */
uint32_t micros(void)
{
    uint32_t ms, cvr, istatus;

    __asm__ volatile("cpsid i" ::: "memory");   /* disable interrupts */
    cvr     = SYST_CVR;
    ms      = systick_millis_count;
    istatus = SCB_ICSR;
    __asm__ volatile("cpsie i" ::: "memory");   /* re-enable interrupts */

    /* If a SysTick interrupt is pending (PENDSTSET) and CVR != 0 the tick
     * has not yet been serviced — treat it as the next millisecond. */
    if ((istatus & SCB_ICSR_PENDSTSET) && cvr != 0) {
        ms++;
    }

    /* elapsed_ticks counts down from RVR to 0; convert to elapsed. */
    uint32_t elapsed_ticks = (996000u - 1u) - cvr;   /* = SYST_RVR - cvr */
    uint32_t us_frac = elapsed_ticks / 996u;           /* 996 ticks per µs */
    return ms * 1000u + us_frac;
}

/* -----------------------------------------------------------------------
 * delay() — spin for the requested number of milliseconds.
 *
 * Phase-0 implementation: simple millis()-based spin loop.  yield() is
 * provided as a weak no-op stub below so this file links standalone.
 * Later phases will override yield() with the EventResponder dispatcher.
 * --------------------------------------------------------------------- */
__attribute__((weak)) void yield(void) {}

void delay(uint32_t msec)
{
    uint32_t start;
    if (msec == 0) return;
    start = millis();
    while ((millis() - start) < msec) {
        yield();
    }
}

/* -----------------------------------------------------------------------
 * delayMicroseconds() — busy-wait using DWT CYCCNT.
 *
 * Defined as a static inline in core_pins.h (uses F_CPU_ACTUAL).  This
 * out-of-line version is a fallback for C translation units that do not
 * include core_pins.h (or that include it without __IMXRT1062__/__IMXRT1176__
 * defined so the inline is not emitted).
 * --------------------------------------------------------------------- */
void delayMicroseconds(uint32_t us)
{
    uint32_t begin  = ARM_DWT_CYCCNT;
    uint32_t cycles = (F_CPU_ACTUAL / 1000000u) * us;
    while ((ARM_DWT_CYCCNT - begin) < cycles) {
        /* busy wait */
    }
}
