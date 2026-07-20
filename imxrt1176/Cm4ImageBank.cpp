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
    if (stageAddr < CM4_ITCM_BACKDOOR_BASE ||
        stageAddr + bytes > CM4_ITCM_BACKDOOR_BASE + CM4_ITCM_BACKDOOR_SIZE)
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
    if (resident_[h] && Multicore.running()) {           /* FAST PATH: VTOR flip */
        Multicore.switchImage(imgs_[h].stageAddr);
        ok = Multicore.running();
    } else {                                             /* STAGE PATH: copy + boot */
        ok = Multicore.begin(imgs_[h].blob, imgs_[h].bytes, imgs_[h].stageAddr);
        if (ok) {
            for (int i = 0; i < n_; i++)                 /* evict same-slot siblings */
                if (imgs_[i].stageAddr == imgs_[h].stageAddr) resident_[i] = false;
            resident_[h] = true;
        }
    }
    if (ok) current_ = h;
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
