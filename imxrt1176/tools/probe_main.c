#include "../imxrt1176.h"
extern uint32_t millis(void);
volatile uint32_t millis_marker __attribute__((used));
volatile uint32_t boot_marker   __attribute__((used));
volatile uint32_t clk_marker    __attribute__((used));
extern volatile uint32_t SystemCoreClock;
int main(void) {
    boot_marker = 0xB00710ADu;
    clk_marker  = SystemCoreClock;
    for (;;) millis_marker = millis();
}
