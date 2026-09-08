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
        volatile uint32_t &rcsr, &rcr1, &rcr2, &rcr3, &rcr4, &rcr5, &rdr0;
        volatile uint32_t &rxd_mux, &rxd_pad, &rxd_select;   // SAI1_RXD[0] = AD_20 (input)
    };
    I2SClass(const hardware_t *hw) : hw(hw) {}
    bool begin(uint32_t sampleRate = 48000);   // v1: only 48000 supported
    void end();
    void write(const int16_t *interleavedLR, size_t nFrames);  // polled, blocking
    // DMA-fed continuous TX. ring = interleaved L,R,L,R... in DMAMEM. refillHalf
    // (optional) is called from the DMA ISR with a pointer to the just-drained
    // half so the caller regenerates it; nullptr = loop the same buffer (static
    // tone). Returns false if no DMA channel is free.
    bool beginDMA(const int16_t *ring, size_t nFrames, void (*refillHalf)(int16_t *half) = nullptr);
    uint32_t dmaBlockCount() const;             // half/complete ISR count (for tests)
    void     read(int16_t *interleavedLR, size_t nFrames);                 // polled capture
    bool     beginReceiveDMA(int16_t *ring, size_t nFrames,
                             void (*onBlock)(int16_t *half) = nullptr);     // continuous capture
    uint32_t rxBlockCount() const;
private:
    void configureSAI();
    bool configured_ = false;
    const hardware_t *hw;
};
extern I2SClass I2S;
/* Audio PLL (ANATOP AI) bring-up, denominator FIXED at 1e6 so the running
 * fraction is named by its numerator alone and audioPllTrimPpm() below always
 * knows what it is trimming.  Known fractions (24 MHz OSC, postDiv /2, clock
 * root /16 -> MCLK = fs*512):
 *     48   kHz: audioPllConfigure(32, 768000)   -- 24*32.768  /2/16 = 24.576 MHz
 *     44.1 kHz: audioPllConfigure(30, 105600)   -- 24*30.1056 /2/16 = 22.5792 MHz
 * The 44.1 kHz one is what the Audio library's AudioOutputI2S::config_i2s()
 * programs on this board (it used to spell the same ratio 1056/10000). */
void audioPllConfigure(uint32_t loopDiv, uint32_t num1e6);

/* NEW-41: +-500 ppm trim of the RUNNING audio PLL (0 = nominal), numerator
 * only -- no re-lock.  Trims whichever fraction audioPllConfigure() last
 * programmed: 32.768 numerator counts per ppm at 48 kHz, 30.1056 at 44.1 kHz.
 * Writes nothing and returns 0 if the PLL was never brought up through
 * audioPllConfigure().  Returns the ppm actually applied (after clamping). */
int32_t audioPllTrimPpm(int32_t ppm);
#endif
