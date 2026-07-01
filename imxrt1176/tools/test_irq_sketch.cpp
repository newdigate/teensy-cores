#include "core_pins.h"
#define SENTINEL_ADDR 0x20001000u          // fixed DTCM slot for monitor xp read
static volatile uint32_t *const sentinel = (uint32_t *)SENTINEL_ADDR;
static void test_isr(void) { *sentinel = 0xC0DE0001u; NVIC_CLEAR_PENDING(IRQ_LPUART12); }
extern "C" int main(void) {
    *sentinel = 0;
    attachInterruptVector(IRQ_LPUART12, &test_isr);
    NVIC_SET_PRIORITY(IRQ_LPUART12, 128);
    NVIC_ENABLE_IRQ(IRQ_LPUART12);
    NVIC_SET_PENDING(IRQ_LPUART12);        // software-trigger; ISR must run
    while (1) { __asm__ volatile("wfi"); }
}
