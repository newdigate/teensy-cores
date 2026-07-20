/* Cm4ImageBank.h - a tiny registry over Multicore.switchImage(): register
 * several build-fixed CM4 images and switch the running CM4 between them by
 * handle. Distinct stage addresses stay co-resident (fast VTOR flip); images
 * sharing a stage address page (re-stage + evict). CM7-side; uses the global
 * Multicore. Public domain (author: Nicholas Newdigate). */
#ifndef Cm4ImageBank_h
#define Cm4ImageBank_h

#include <stdint.h>
#include <stddef.h>
#include "imxrt1176.h"     /* CM4_BOOT_ADDRESS */

/* The CM4 ITCM backdoor window (code images only; the DTCM half is data). */
#define CM4_ITCM_BACKDOOR_BASE   CM4_BOOT_ADDRESS   /* 0x20200000 */
#define CM4_ITCM_BACKDOOR_SIZE   0x20000u           /* 128 KB      */

#ifdef __cplusplus

struct Cm4ImageDesc {
    const void *blob;       /* embedded image in flash (e.g. cm4_ib_a[]) */
    uint32_t    bytes;      /* image length */
    uint32_t    stageAddr;  /* build-fixed backdoor alias = image's link base */
    const char *name;       /* optional, debug only (may be nullptr) */
};

class Cm4ImageBank {
public:
    /* Register an image. Returns a handle >= 0, or -1 on error: table full,
     * stageAddr/bytes outside the 128K ITCM backdoor window, or a partial
     * overlap with a *different* slot (same stageAddr = same slot, allowed). */
    int  add(const void *blob, uint32_t bytes, uint32_t stageAddr,
             const char *name = nullptr);

    /* Boot/switch the CM4 into `handle`. Fast VTOR flip (Multicore.switchImage)
     * if it is already staged in its slot and the CM4 is running; otherwise
     * (re)stage + boot via Multicore.begin(), evicting any same-slot image.
     * Returns true once the CM4 has left reset (hardware-level ready), false
     * on a bad handle or a reset timeout. */
    bool switchTo(int handle);

    int  current() const { return current_; }   /* running handle, -1 if none */
    bool isResident(int handle) const;           /* its slot currently holds it */
    int  count() const { return n_; }
    const char *name(int handle) const;          /* debug label, "" if none */

private:
    static constexpr int MAX_IMAGES = 16;        /* cap on *registered* images */
    Cm4ImageDesc imgs_[MAX_IMAGES];
    bool         resident_[MAX_IMAGES] = { false };
    int          n_ = 0;
    int          current_ = -1;
};

#endif /* __cplusplus */
#endif /* Cm4ImageBank_h */
