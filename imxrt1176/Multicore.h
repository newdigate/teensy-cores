/* Multicore.h - CM7-side API to boot the i.MX RT1176's Cortex-M4 core.
 *
 * The CM7 stages a CM4 image into the CM4 TCM backdoor (system alias
 * 0x20200000), programs the CM4 reset vector into IOMUXC_LPSR_GPR0/GPR1, and
 * releases the core through the SRC.  This is a clean-room API written against
 * RM ch.25 (SRC) + ch.12 (IOMUXC_LPSR_GPR); the boot sequence matches NXP
 * MCMGR (mcmgr_internal_core_api_imxrt1170.c) and Zephyr (imxrt11xx/soc.c) and
 * was HW-verified on the EVKB by evkb/dualcore_mu_test (2026-07-16).
 *
 * The CM4 image must be a Cortex-M vector table (initial SP + reset PC) at its
 * start, linked for the CM4-private ITCM alias 0x1FFE0000 (the CM4 resolves
 * its own absolute pointers there), but STAGED and BOOTED at the system alias
 * 0x20200000 -- both address the same physical CM4 TCM.  A VTOR in the
 * CM4-private window does NOT boot on silicon.
 *
 * Use the messaging unit (MessagingUnit.h / the MU global) to talk to the CM4
 * once it is running.
 *
 * Public domain (author: Nicholas Newdigate).
 */
#ifndef Multicore_h
#define Multicore_h

#include <stdint.h>
#include <stddef.h>
#include "imxrt1176.h"

#ifdef __cplusplus

class MulticoreClass {
public:
    /* Stage `image` (`bytes`, rounded up to a 32-bit word) into the CM4 TCM
     * backdoor at `stageAddr`, program VTOR = `stageAddr`, and release the
     * CM4.  `stageAddr` must be the system backdoor 0x20200000 (default).
     * The first call performs the write-1-only BT_RELEASE_M4 boot edge;
     * because that bit cannot be cleared on silicon, the CM4 cannot afterwards
     * be re-held -- use restart() to reboot it.  Returns true once the CM4
     * leaves reset (STAT_M4CORE.UNDER_RST -> 0) within the internal timeout. */
    bool begin(const void *image, size_t bytes,
               uint32_t stageAddr = CM4_BOOT_ADDRESS);

    /* Reboot the (already-released) CM4 from its current VTOR/image by pulsing
     * CTRL_M4CORE.SW_RESET.  HW-verified (dualcore_mu_test hello2).  No-op if
     * begin() has not run. */
    void restart();

    /* Reboot the CM4 into an ALREADY-STAGED image at a (possibly different)
     * backdoor address by reprogramming GPR0/1 and re-pulsing SW_RESET WITHOUT
     * re-copying the image. begin() must have released the CM4 first (no-op
     * otherwise). Enables switching the boot VTOR between two co-resident CM4
     * images with no re-staging. */
    void switchImage(uint32_t stageAddr);

    /* True while the CM4 is running (STAT_M4CORE.UNDER_RST == 0). */
    bool running();

    /* The system address the CM4 was booted from (0 before begin()). */
    uint32_t bootAddress() const { return boot_addr_; }

private:
    uint32_t boot_addr_ = 0;
    bool released_ = false;
};

extern MulticoreClass Multicore;

#endif /* __cplusplus */
#endif /* Multicore_h */
