#include "I2S.h"
#include "DMAChannel.h"

static inline void ai_udelay(volatile uint32_t n) { while (n--) __asm__ volatile("nop"); }

/* One ANATOP AI indirect write to the Audio PLL register file. Matches the
 * SDK ANATOP_AI_Write (fsl_anatop_ai.c) and the QEMU imxrt_anadig AI model
 * (toggle bit8 -> done bit9). */
static void ai_write(uint8_t aiAddr, uint32_t wdata) {
    uint32_t pre = ANADIG_MISC_AI_CTRL_AUDIO & ANADIG_AI_TOGGLE_DONE;
    ANADIG_MISC_AI_CTRL_AUDIO &= ~ANADIG_AI_RWB;                 // write mode
    ANADIG_MISC_AI_CTRL_AUDIO = (ANADIG_MISC_AI_CTRL_AUDIO & ~0xFFu) | aiAddr;
    ANADIG_MISC_AI_WDATA_AUDIO = wdata;
    ANADIG_MISC_AI_CTRL_AUDIO ^= ANADIG_AI_TOGGLE;              // request
    while ((ANADIG_MISC_AI_CTRL_AUDIO & ANADIG_AI_TOGGLE_DONE) == pre) { }
}

/* AI register sub-addresses (fsl_anatop_ai.h kAI_PLLAUDIO_*): CTRL0=0x00,
 * CTRL0_SET=0x04, CTRL0_CLR=0x08, CTRL2(num)=0x20, CTRL3(denom)=0x30.
 * CTRL0 bits: loopDiv[6:0], HOLD_RING_OFF b13, POWER_UP b14, ENABLE b15,
 * BYPASS b16, PLL_REG_EN b22, POST_DIV[27:25]. postDiv=1 -> divide by 2.
 *
 * The DENOMINATOR is fixed at 1e6 for every caller, so a fraction is named by
 * its numerator alone and audioPllTrimPpm() below always knows what it is
 * trimming.  1e6 (rather than the 1e3/1e4 the SDK tables use) is what gives
 * the trim a fine enough step: one numerator count is 1/den of the loop ratio,
 * i.e. ~30 ppm at den=1e3 but ~0.03 ppm at den=1e6.  Both NUM and DENOM are
 * 30-bit fields (RM 15.9.4.18/19), so 1e6 fits with room to spare.
 *
 * Known fractions (24 MHz OSC reference, postDiv=1 -> VCO/2, clock root /16):
 *   48   kHz: audioPllConfigure(32, 768000)  -- 24*32.768   = 786.432 MHz;
 *             /2 = 393.216 MHz; /16 = 24.576 MHz = 48000*512.
 *   44.1 kHz: audioPllConfigure(30, 105600)  -- 24*30.1056  = 722.5344 MHz;
 *             /2 = 361.2672 MHz; /16 = 22.5792 MHz = 44100*512.
 *             (0.1056 exactly -- the same ratio the Audio library's
 *             AudioOutputI2S::config_i2s() used to program as 1056/10000.)
 *
 * The running fraction is recorded so audioPllTrimPpm() trims whichever one
 * is live rather than assuming 48 kHz. */
static uint32_t audio_pll_loop_div = 0;
static uint32_t audio_pll_nom_num  = 0;
static bool     audio_pll_running  = false;

void audioPllConfigure(uint32_t loopDiv, uint32_t num1e6) {
    audio_pll_loop_div = loopDiv;
    audio_pll_nom_num  = num1e6;
    audio_pll_running  = true;
    ai_write(0x04, (1u << 16));                 // CTRL0_SET: BYPASS
    ANADIG_PLL_AUDIO_CTRL |= ANADIG_PLL_AUDIO_CTRL_ENABLE_CLK;
    ai_write(0x30, 1000000);                    // denominator (always 1e6)
    ai_write(0x20, num1e6);                     // numerator
    ai_write(0x08, 0x7Fu);                      // CTRL0_CLR: clear loopDiv field
    ai_write(0x04, loopDiv & 0x7Fu);            // CTRL0_SET: loopDiv
    ai_write(0x08, 0x7u << 25);                 // CTRL0_CLR: clear postDiv field
    ai_write(0x04, (1u << 25));                 // CTRL0_SET: postDiv=1 (div by 2)
    ai_write(0x04, (1u << 22));                 // CTRL0_SET: PLL_REG_EN
    ai_udelay(20000);                           // >=100us settle
    ai_write(0x04, (1u << 14) | (1u << 13));    // CTRL0_SET: POWER_UP | HOLD_RING_OFF
    ai_udelay(45000);                           // >=225us
    ai_write(0x08, (1u << 13));                 // CTRL0_CLR: HOLD_RING_OFF
    uint32_t guard = 2000000;
    while (!(ANADIG_PLL_AUDIO_CTRL & ANADIG_PLL_AUDIO_CTRL_STABLE) && guard--) { }
    ai_write(0x04, (1u << 15));                 // CTRL0_SET: ENABLE (clk out)
    ANADIG_PLL_AUDIO_CTRL &= ~ANADIG_PLL_AUDIO_CTRL_GATE;  // ungate
    ai_write(0x08, (1u << 16));                 // CTRL0_CLR: clear BYPASS
}

/* This core's own 48 kHz template (unchanged register writes). */
static void sai1_audio_pll_init(void) {
    audioPllConfigure(32, 768000);
}

/* NEW-41: trim the running audio PLL by `ppm` (clamped to +-500) -- the A2DP
 * sink servos its SAI clock onto the source's delivery rate with this.
 * Numerator only (the denominator stays at the 1e6 audioPllConfigure()
 * programmed); the fractional divider takes the new value without a
 * re-power/re-lock, which is the same single CTRL2 write the SDK's
 * ANATOP_PllConfigure() uses to load the numerator (fsl_clock.c:694-695,
 * PLL_AI_CTRL3_REG=0x30 denom then PLL_AI_CTRL2_REG=0x20 num).
 *
 * Arithmetic: the loop ratio is R = loopDiv + num/1e6.  A relative change of
 * `ppm` parts per million is dR = R * ppm * 1e-6, and since num = R_frac*1e6,
 *   d(num) = dR * 1e6 = R * ppm.
 * So the counts-per-ppm step IS the loop ratio: 32.768 for the 48 kHz
 * fraction, 30.1056 for the 44.1 kHz one.  At the +-500 clamp the numerator
 * moves by at most ~16400 counts, comfortably inside the 30-bit NUM field
 * either way.
 *
 * PRECONDITION: the audio PLL must already have been brought up through
 * audioPllConfigure(), which is what records the nominal fraction.  That
 * happens lazily from I2SClass::configureSAI() (first I2S.begin()/beginDMA()/
 * beginReceiveDMA()) for the core's own 48 kHz path, and from
 * AudioOutputI2S::config_i2s() (Audio library) for the 44.1 kHz one.  If
 * nothing has been configured yet this writes NOTHING and returns 0 --
 * writing a numerator against an unknown denominator is worse than not
 * trimming.  Note the Audio library's interrupt-driven nodes
 * (AudioOutputI2SInt/AudioInputI2SInt) bring the same PLL up through their own
 * freestanding copy in Audio/sai1176.c (it must compile for bare-metal CM4
 * images, so it cannot call in here); a graph built on those is NOT trimmable
 * and this function correctly declines rather than guessing.
 *
 * Returns the ppm actually applied (i.e. after clamping), or 0 if declined.
 */
int32_t audioPllTrimPpm(int32_t ppm) {
    if (!audio_pll_running) return 0;
    if (ppm > 500) ppm = 500;
    if (ppm < -500) ppm = -500;
    int64_t ratio1e6 = (int64_t)audio_pll_loop_div * 1000000 + (int64_t)audio_pll_nom_num;
    int32_t num = (int32_t)audio_pll_nom_num + (int32_t)((ppm * ratio1e6) / 1000000);
    ai_write(0x20, (uint32_t)num);
    return ppm;
}

void I2SClass::configureSAI() {
    if (configured_) return;                     // idempotent: full-duplex calls it twice
    configured_ = true;
    sai1_audio_pll_init();
    hw->clock_root = hw->clock_root_val;         // mux 4 (Audio PLL), div 16
    hw->lpcg = 1u;                               // ungate SAI1
    // Pin mux (ALT0) + pad ctl 0x02; MCLK as output via GPR0. SION(0x10) on the
    // clock pins per the SDK pin_mux.
    hw->mclk_mux = 0x10u; hw->data_mux = 0u; hw->bclk_mux = 0x10u; hw->sync_mux = 0x10u;
    hw->mclk_pad = 0x02u; hw->data_pad = 0x02u; hw->bclk_pad = 0x02u; hw->sync_pad = 0x02u;
    hw->gpr0 |= IOMUXC_GPR_GPR0_SAI1_MCLK_DIR;
    // Reset TX, then configure I2S 16-bit stereo master.
    hw->tcsr = SAI_TCSR_SR; hw->tcsr = 0u;
    hw->tcsr = SAI_TCSR_FR; hw->tcsr = 0u;
    hw->tmr = 0u;
    hw->tcr1 = 16u;                              // FIFO watermark
    hw->tcr2 = SAI_TCR2_MSEL(1) | SAI_TCR2_BCD | SAI_TCR2_BCP | SAI_TCR2_DIV(7);
    hw->tcr3 = SAI_TCR3_TCE(1);                  // enable channel 0
    hw->tcr4 = SAI_TCR4_FRSZ(1) | SAI_TCR4_SYWD(15) | SAI_TCR4_MF |
               SAI_TCR4_FSD | SAI_TCR4_FSE | SAI_TCR4_FSP |
               (1u << 28);   // FCONT: keep the bit clock running through a FIFO
                             // underrun (a polled TX briefly underruns at start;
                             // without FCONT the SAI halts and never recovers).
    hw->tcr5 = SAI_TCR5_WNW(15) | SAI_TCR5_W0W(15) | SAI_TCR5_FBT(15);
    // RX: synchronous to TX (shares BCLK/FS); RXD is an input -> select_input.
    hw->rxd_mux = 0u;                            // ALT0
    hw->rxd_pad = 0x02u;
    hw->rxd_select = 0u;                         // daisy: select AD_20
    hw->rcsr = SAI_RCSR_SR; hw->rcsr = 0u;
    hw->rcsr = SAI_RCSR_FR; hw->rcsr = 0u;
    hw->rcr1 = 0u;                               // watermark 0 -> FRF at >=1 sample
    hw->rcr2 = SAI_RCR2_SYNC(1);                 // synchronous to transmitter
    hw->rcr3 = SAI_RCR3_RCE(1);
    hw->rcr4 = SAI_RCR4_FRSZ(1) | SAI_RCR4_SYWD(15) | SAI_RCR4_MF |
               SAI_RCR4_FSE | SAI_RCR4_FSP;      // no FSD (uses TX sync), no FCONT
    hw->rcr5 = SAI_RCR5_WNW(15) | SAI_RCR5_W0W(15) | SAI_RCR5_FBT(15);
}

bool I2SClass::begin(uint32_t sampleRate) {
    if (sampleRate != 48000) return false;       // v1: 48k only
    configureSAI();
    // Pre-fill the FIFO with silence before enabling TX so the first frame does
    // not underrun before write() is called.
    for (int i = 0; i < 16; i++) hw->tdr0 = 0u;
    hw->tcsr |= SAI_TCSR_TE | SAI_TCSR_BCE;      // enable transmitter + bit clock
    return true;
}

void I2SClass::end() { hw->tcsr = 0u; hw->rcsr = 0u; hw->lpcg = 0u; configured_ = false; }

void I2SClass::write(const int16_t *s, size_t n) {
    for (size_t i = 0; i < n; i++) {
        uint32_t g = 2000000;                    // FIFO-room guard (bounded)
        while (!(hw->tcsr & SAI_TCSR_FWF) && g--) { }
        hw->tdr0 = (uint16_t)s[2*i + 0];         // left
        g = 2000000;
        while (!(hw->tcsr & SAI_TCSR_FWF) && g--) { }
        hw->tdr0 = (uint16_t)s[2*i + 1];         // right
    }
}

void I2SClass::read(int16_t *s, size_t n) {
    hw->rcsr |= SAI_RCSR_RE;                      // enable the receiver (idempotent)
    for (size_t i = 0; i < n; i++) {
        uint32_t g = 2000000;
        while (!(hw->rcsr & SAI_RCSR_FRF) && g--) { }
        s[2*i + 0] = (int16_t)hw->rdr0;          // left
        g = 2000000;
        while (!(hw->rcsr & SAI_RCSR_FRF) && g--) { }
        s[2*i + 1] = (int16_t)hw->rdr0;          // right
    }
}

static DMAChannel i2s_dma(false);
static const int16_t *i2s_ring;
static size_t i2s_ring_frames;
static void (*i2s_refill)(int16_t *);
static volatile uint32_t i2s_blocks;

static void i2s_dma_isr() {
    i2s_dma.clearInterrupt();
    i2s_blocks++;
    if (i2s_refill) {
        // Which half just drained? SADDR has advanced past it.
        uint32_t saddr    = (uint32_t)i2s_dma.TCD->SADDR;
        uint32_t halfByte = (uint32_t)(i2s_ring_frames * 2u * sizeof(int16_t) / 2u);
        if (saddr < (uint32_t)i2s_ring + halfByte)
            i2s_refill((int16_t *)i2s_ring + i2s_ring_frames);   // 2nd half drained
        else
            i2s_refill((int16_t *)i2s_ring);                     // 1st half drained
    }
}

bool I2SClass::beginDMA(const int16_t *ring, size_t nFrames, void (*refillHalf)(int16_t *)) {
    configureSAI();                              // SAI up, but TX not yet enabled
    i2s_ring = ring; i2s_ring_frames = nFrames; i2s_refill = refillHalf; i2s_blocks = 0;
    i2s_dma.begin();
    if (i2s_dma.TCD == 0) return false;          // no channel free
    i2s_dma.sourceBuffer((const uint16_t *)ring, (unsigned int)(nFrames * 2u * sizeof(int16_t)));
    i2s_dma.destination(*(volatile uint16_t *)&SAI1_TDR0);
    i2s_dma.interruptAtHalf();
    i2s_dma.interruptAtCompletion();
    i2s_dma.triggerAtHardwareEvent(DMAMUX_SOURCE_SAI1_TX);
    i2s_dma.attachInterrupt(i2s_dma_isr);
    i2s_dma.enable();
    hw->tcsr |= SAI_TCSR_TE | SAI_TCSR_BCE | SAI_TCSR_FRDE;  // TX + bit clock + FIFO DMA request
    return true;
}

uint32_t I2SClass::dmaBlockCount() const { return i2s_blocks; }

static DMAChannel i2s_rx_dma(false);
static int16_t *i2s_rx_ring;
static size_t i2s_rx_frames;
static void (*i2s_rx_onblock)(int16_t *);
static volatile uint32_t i2s_rx_blocks;

static void i2s_rx_isr() {
    i2s_rx_dma.clearInterrupt();
    i2s_rx_blocks++;
    if (i2s_rx_onblock) {
        uint32_t daddr    = (uint32_t)i2s_rx_dma.TCD->DADDR;
        uint32_t halfByte = (uint32_t)(i2s_rx_frames * 2u * sizeof(int16_t) / 2u);
        if (daddr < (uint32_t)i2s_rx_ring + halfByte)
            i2s_rx_onblock(i2s_rx_ring + i2s_rx_frames);   // 2nd half just filled
        else
            i2s_rx_onblock(i2s_rx_ring);                   // 1st half just filled
    }
}

bool I2SClass::beginReceiveDMA(int16_t *ring, size_t nFrames, void (*onBlock)(int16_t *)) {
    configureSAI();                              // idempotent
    i2s_rx_ring = ring; i2s_rx_frames = nFrames; i2s_rx_onblock = onBlock; i2s_rx_blocks = 0;
    i2s_rx_dma.begin();
    if (i2s_rx_dma.TCD == 0) return false;       // no channel free
    i2s_rx_dma.source(*(volatile uint16_t *)&SAI1_RDR0);                 // fixed FIFO source
    i2s_rx_dma.destinationBuffer((uint16_t *)ring, (unsigned int)(nFrames * 2u * sizeof(int16_t)));
    i2s_rx_dma.interruptAtHalf();
    i2s_rx_dma.interruptAtCompletion();
    i2s_rx_dma.triggerAtHardwareEvent(DMAMUX_SOURCE_SAI1_RX);
    i2s_rx_dma.attachInterrupt(i2s_rx_isr);
    i2s_rx_dma.enable();
    hw->tcsr |= SAI_TCSR_TE | SAI_TCSR_BCE;      // ensure the shared clock is running
    hw->rcsr |= SAI_RCSR_RE | SAI_RCSR_FRDE;     // RX + FIFO DMA request
    return true;
}
uint32_t I2SClass::rxBlockCount() const { return i2s_rx_blocks; }
