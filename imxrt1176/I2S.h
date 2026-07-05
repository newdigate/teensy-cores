#ifndef I2S_h
#define I2S_h
#include <stdint.h>
#include <stddef.h>
#include "imxrt1176.h"
#include "core_pins.h"

class I2SClass {
public:
    struct hardware_t {
        volatile uint32_t &tcsr, &tcr1, &tcr2, &tcr3, &tcr4, &tcr5, &tdr0, &tfr0, &tmr;
        volatile uint32_t &lpcg;          // SAI1 clock gate
        volatile uint32_t &clock_root;    // SAI1 clock root
        uint32_t clock_root_val;          // mux(4)|div(15) packed
        volatile uint32_t &mclk_mux, &data_mux, &bclk_mux, &sync_mux;
        volatile uint32_t &mclk_pad, &data_pad, &bclk_pad, &sync_pad;
        volatile uint32_t &gpr0;          // IOMUXC_GPR_GPR0 (MCLK_DIR)
    };
    I2SClass(const hardware_t *hw) : hw(hw) {}
    bool begin(uint32_t sampleRate = 48000);   // v1: only 48000 supported
    void end();
    void write(const int16_t *interleavedLR, size_t nFrames);  // polled, blocking
private:
    const hardware_t *hw;
};
extern I2SClass I2S;
#endif
