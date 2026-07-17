/* Multicore.cpp - CM7-side CM4 boot for the i.MX RT1176.  See Multicore.h.
 *
 * Boot sequence (clean-room vs RM ch.25/ch.12; matches NXP MCMGR + Zephyr,
 * HW-verified by evkb/dualcore_mu_test 2026-07-16):
 *   1. copy the image into the CM4 TCM backdoor (0x20200000), dsb/isb;
 *   2. IOMUXC_LPSR_GPR0/GPR1 = CM4 reset VTOR (system address);
 *   3. defensively ungate the CM4 clock (CCM LPCG1);
 *   4. pulse SRC.CTRL_M4CORE.SW_RESET;
 *   5. set SRC.SCR.BT_RELEASE_M4 (write-1-only boot edge, first call only).
 * The CM4 then fetches its initial SP+PC from the vector table at VTOR.
 *
 * Public domain (author: Nicholas Newdigate).
 */
#include "Multicore.h"

/* Bounded settle poll: ~300k reads is well beyond the CM4's microsecond-scale
 * reset time on silicon and a fraction of a second in QEMU. */
#define MULTICORE_SETTLE_SPINS 300000u

MulticoreClass Multicore;

static inline void multicore_barrier(void)
{
    __asm__ volatile("dsb 0xF\n isb 0xF\n" ::: "memory");
}

bool MulticoreClass::begin(const void *image, size_t bytes, uint32_t stageAddr)
{
    /* 1. Stage the image into the CM4 TCM backdoor.  A little-endian word
     * assembly copes with an unaligned or non-word-multiple source; the target
     * holds the CM4 vector table (initial SP + reset PC), so it must be 4-byte
     * aligned -- 0x20200000 is.  dualcore_mu_test proved dsb/isb is sufficient
     * for the CM4 to see the bytes on this silicon (no explicit cache clean
     * needed for this OCRAM-alias region). */
    volatile uint32_t *dst = (volatile uint32_t *)stageAddr;
    const uint8_t *src = (const uint8_t *)image;
    size_t words = (bytes + 3u) / 4u;
    for (size_t i = 0; i < words; i++) {
        uint32_t w = 0;
        for (unsigned b = 0; b < 4u; b++) {
            size_t idx = i * 4u + b;
            if (idx < bytes) {
                w |= (uint32_t)src[idx] << (8u * b);
            }
        }
        dst[i] = w;
    }
    multicore_barrier();

    /* 2. Program the CM4 reset vector.  GPR0 carries VTOR[15:3] (low 3 bits
     * forced 0), GPR1 carries VTOR[31:16].  Must be the system address. */
    IOMUXC_LPSR_GPR0 = stageAddr & 0xFFF8u;
    IOMUXC_LPSR_GPR1 = (stageAddr >> 16) & 0xFFFFu;

    /* 3. Defensively ungate the CM4 core clock (boot ROM already does this;
     * the SDK/Zephyr rely on that -- writing 1 is a harmless belt-and-braces). */
    CCM_LPCG1_DIRECT |= 1u;
    multicore_barrier();

    /* 4. Pulse the CM4 slice software reset (self-clearing).  Before release
     * this is a no-op; after release it reboots the CM4 from the VTOR. */
    SRC_CTRL_M4CORE = SRC_CTRL_M4CORE_SW_RESET;

    /* 5. Release the CM4 from boot hold (write-1-only rising edge).  Only the
     * first begin() produces the release edge; the bit cannot be cleared. */
    if (!released_) {
        SRC_SCR |= SRC_SCR_BT_RELEASE_M4;
        released_ = true;
    }
    boot_addr_ = stageAddr;

    for (volatile uint32_t n = MULTICORE_SETTLE_SPINS; n; n--) {
        if (running()) {
            break;
        }
    }
    return running();
}

void MulticoreClass::restart()
{
    if (!released_) {
        return;
    }
    SRC_CTRL_M4CORE = SRC_CTRL_M4CORE_SW_RESET;
    for (volatile uint32_t n = MULTICORE_SETTLE_SPINS; n; n--) {
        if (running()) {
            break;
        }
    }
}

bool MulticoreClass::running()
{
    return (SRC_STAT_M4CORE & SRC_STAT_M4CORE_UNDER_RST) == 0;
}
