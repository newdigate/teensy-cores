#include "I2S.h"

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
 * BYPASS b16, PLL_REG_EN b22, POST_DIV[27:25]. Values below are the encoding
 * for {loopDiv=32, num=768, den=1000, postDiv=1 -> divide-by-2 -> 393.216 MHz}. */
static void sai1_audio_pll_init(void) {
    ai_write(0x04, (1u << 16));                 // CTRL0_SET: BYPASS
    ANADIG_PLL_AUDIO_CTRL |= ANADIG_PLL_AUDIO_CTRL_ENABLE_CLK;
    ai_write(0x30, 1000);                       // denominator
    ai_write(0x20, 768);                        // numerator
    ai_write(0x08, 0x7Fu);                      // CTRL0_CLR: clear loopDiv field
    ai_write(0x04, 32u & 0x7Fu);                // CTRL0_SET: loopDiv=32
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

bool I2SClass::begin(uint32_t sampleRate) {
    if (sampleRate != 48000) return false;       // v1: 48k only
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
               SAI_TCR4_FSD | SAI_TCR4_FSE | SAI_TCR4_FSP;
    hw->tcr5 = SAI_TCR5_WNW(15) | SAI_TCR5_W0W(15) | SAI_TCR5_FBT(15);
    hw->tcsr = SAI_TCSR_TE | SAI_TCSR_BCE;       // enable transmitter + bit clock
    return true;
}

void I2SClass::end() { hw->tcsr = 0u; hw->lpcg = 0u; }

void I2SClass::write(const int16_t *s, size_t n) { (void)s; (void)n; }  // Task 5
