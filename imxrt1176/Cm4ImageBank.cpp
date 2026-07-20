/* Cm4ImageBank.cpp - see Cm4ImageBank.h. Bookkeeping only; the boot machinery
 * is the HW-verified global Multicore. Public domain (N. Newdigate). */
#include "Cm4ImageBank.h"
#include "Multicore.h"

/* [a, a+alen) intersects [b, b+blen) ? */
static inline bool ranges_overlap(uint32_t a, uint32_t alen,
                                  uint32_t b, uint32_t blen)
{
    return a < b + blen && b < a + alen;
}

int Cm4ImageBank::add(const void *blob, uint32_t bytes, uint32_t stageAddr,
                      const char *name)
{
    if (n_ >= MAX_IMAGES) return -1;
    /* Fit within the 128K ITCM backdoor window, overflow-safe: stageAddr becomes
     * untrusted once images are loaded from storage (spec Future/SD path), so
     * avoid the wrap in `stageAddr + bytes`. */
    if (stageAddr < CM4_ITCM_BACKDOOR_BASE ||
        bytes > CM4_ITCM_BACKDOOR_SIZE ||
        stageAddr - CM4_ITCM_BACKDOOR_BASE > CM4_ITCM_BACKDOOR_SIZE - bytes)
        return -1;
    for (int i = 0; i < n_; i++)                 /* partial overlap with a */
        if (imgs_[i].stageAddr != stageAddr &&   /* *different* slot = layout bug */
            ranges_overlap(stageAddr, bytes, imgs_[i].stageAddr, imgs_[i].bytes))
            return -1;
    imgs_[n_].blob = blob; imgs_[n_].bytes = bytes;
    imgs_[n_].stageAddr = stageAddr; imgs_[n_].name = name;
    resident_[n_] = false;
    return n_++;
}

bool Cm4ImageBank::switchTo(int h)
{
    if (h < 0 || h >= n_) return false;
    bool ok;
    if (resident_[h] && Multicore.running()) {           /* FAST PATH: VTOR flip, no copy */
        Multicore.switchImage(imgs_[h].stageAddr);
        ok = Multicore.running();
    } else {                                             /* STAGE PATH: copy + boot */
        ok = Multicore.begin(imgs_[h].blob, imgs_[h].bytes, imgs_[h].stageAddr);
        /* begin() copies h into its slot and pulses SW_RESET UNCONDITIONALLY,
         * before its settle-timeout -- so h is physically resident and any
         * same-slot sibling is evicted regardless of whether the boot confirmed.
         * Track that even when ok==false, or a later fast-flip could reboot a
         * slot whose bytes we already replaced (a silent wrong-image boot). */
        for (int i = 0; i < n_; i++)                     /* evict same-slot siblings */
            if (imgs_[i].stageAddr == imgs_[h].stageAddr) resident_[i] = false;
        resident_[h] = true;
    }
    current_ = ok ? h : -1;   /* both paths pulse reset; a failed boot leaves the
                               * running image unknown, not the stale previous one */
    return ok;
}

bool Cm4ImageBank::isResident(int h) const
{
    return (h >= 0 && h < n_) ? resident_[h] : false;
}

const char *Cm4ImageBank::name(int h) const
{
    if (h < 0 || h >= n_ || !imgs_[h].name) return "";
    return imgs_[h].name;
}
