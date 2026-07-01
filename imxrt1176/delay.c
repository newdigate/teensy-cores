/* Teensyduino Core Library - MIMXRT1176-EVKB Phase-0 timing primitives.
 *
 * Provides millis(), micros(), delay(), and delayMicroseconds() for the
 * NXP i.MX RT1176 (Cortex-M7) port.
 *
 * Phase-0 time base is the free-running DWT cycle counter (ARM_DWT_CYCCNT),
 * NOT the SysTick interrupt.  On RT1176 silicon the SysTick tick ISR does not
 * reliably advance a millisecond counter (extensively investigated; the counter
 * is corrupted rather than incremented, exact mechanism not isolated), so a
 * SysTick-driven millis()/delay() hung.  DWT CYCCNT needs no interrupt and is
 * verified working on hardware (it drives blink), so all timing is derived
 * from it.  DWT is enabled by systick_init() in startup.c.
 */

#include <stdint.h>
#include "imxrt1176.h"

/* Legacy SysTick counter, kept for source compatibility; no longer the time
 * base.  The SysTick ISR still increments it but nothing reads it. */
volatile uint32_t systick_millis_count = 0;

/* Live core clock (Hz).  Set to 996 MHz; set_arm_clock_rt1176() may update it. */
volatile uint32_t F_CPU_ACTUAL  = 996000000u;
volatile uint32_t F_BUS_ACTUAL  = 996000000u;
volatile uint32_t scale_cpu_cycles_to_microseconds = 0;

/* ---------------------------------------------------------------------------
 * 64-bit-extended cycle count from the 32-bit DWT CYCCNT.  Any timing call
 * maintains the accumulator, so it is exact as long as a timing function is
 * called at least once per CYCCNT wrap (~4.3 s @ 996 MHz) - which delay() and
 * a typical loop() ensure.
 * ------------------------------------------------------------------------- */
static volatile uint64_t cyc_accum = 0;
static volatile uint32_t cyc_last  = 0;

static uint64_t cycles64(void)
{
    uint32_t now = ARM_DWT_CYCCNT;
    cyc_accum += (uint32_t)(now - cyc_last);   /* correct across one 32-bit wrap */
    cyc_last = now;
    return cyc_accum;
}

uint32_t millis(void)
{
    return (uint32_t)(cycles64() / (uint64_t)(F_CPU_ACTUAL / 1000u));
}

uint32_t micros(void)
{
    return (uint32_t)(cycles64() / (uint64_t)(F_CPU_ACTUAL / 1000000u));
}

/* yield() weak no-op so this file links standalone; later phases override it. */
__attribute__((weak)) void yield(void) {}

/* delay() spins on the DWT-backed millis(); each call maintains the cycle
 * accumulator so the time base cannot drift while delay() runs. */
void delay(uint32_t msec)
{
    uint32_t start = millis();
    while ((millis() - start) < msec) {
        yield();
    }
}

/* delayMicroseconds() busy-waits directly on DWT CYCCNT. */
void delayMicroseconds(uint32_t us)
{
    uint32_t begin  = ARM_DWT_CYCCNT;
    uint32_t cycles = (F_CPU_ACTUAL / 1000000u) * us;
    (void)cycles64();                          /* keep the 64-bit base fresh */
    while ((ARM_DWT_CYCCNT - begin) < cycles) {
        /* busy wait */
    }
}
