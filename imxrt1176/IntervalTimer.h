#pragma once
#include <stdint.h>
#include "imxrt1176.h"    // PIT1_*, CCM_*, NVIC_* macros
#include "core_pins.h"    // IRQ_PIT1, attachInterruptVector, NVIC_NUM_INTERRUPTS

class IntervalTimer {
public:
    IntervalTimer() : channel(-1), nvic_priority(128) {}
    ~IntervalTimer() { end(); }

    bool begin(void (*funct)(void), unsigned int us) { return beginPeriod(funct, (double)us); }
    bool begin(void (*funct)(void), int us)          { return us < 0 ? false : beginPeriod(funct, (double)us); }
    bool begin(void (*funct)(void), float us)        { return beginPeriod(funct, (double)us); }
    bool begin(void (*funct)(void), double us)       { return beginPeriod(funct, us); }

    void update(unsigned int us) { updatePeriod((double)us); }
    void update(int us)          { if (us >= 0) updatePeriod((double)us); }
    void update(float us)        { updatePeriod((double)us); }
    void update(double us)       { updatePeriod(us); }

    void end();
    void priority(uint8_t n);
    operator IRQ_NUMBER_t() { return channel >= 0 ? IRQ_PIT1 : (IRQ_NUMBER_t)NVIC_NUM_INTERRUPTS; }

private:
    int channel;              // 0..3 when allocated, -1 otherwise
    uint8_t nvic_priority;
    bool beginPeriod(void (*funct)(void), double us);
    void updatePeriod(double us);
    static uint32_t ldvalFromMicros(double us);  // returns LDVAL, 0 == invalid
};
