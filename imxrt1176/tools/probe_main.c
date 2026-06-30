#include "../imxrt1176.h"
extern uint32_t millis(void);
volatile uint32_t millis_marker __attribute__((used));
int main(void) { for(;;) millis_marker = millis(); }
