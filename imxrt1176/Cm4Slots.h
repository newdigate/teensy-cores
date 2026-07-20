/* Cm4Slots.h - uniform CM4 ITCM slot layout, the single source of truth shared
 * by the build (linker ORIGIN, via teensy_add_cm4_slot_image) and the runtime
 * (the backdoor stage address the CM7 hands Cm4ImageBank.add()). CM4_SLOT_SIZE
 * is supplied by the build (-DCM4_SLOT_SIZE=...) so both agree. Public domain. */
#ifndef Cm4Slots_h
#define Cm4Slots_h

#include "Cm4ImageBank.h"   /* CM4_ITCM_BACKDOOR_BASE */

#ifndef CM4_SLOT_SIZE
#error "CM4_SLOT_SIZE must be defined by the build (e.g. -DCM4_SLOT_SIZE=0x1000)"
#endif

/* Backdoor stage address of ITCM slot k (what Cm4ImageBank.add() consumes).
 * The matching CM4 image is linked at 0x1FFE0000 + k*CM4_SLOT_SIZE by
 * teensy_add_cm4_slot_image; that private-ITCM base aliases this backdoor. */
#define CM4_SLOT_STAGE(k)   (CM4_ITCM_BACKDOOR_BASE + (uint32_t)(k) * (CM4_SLOT_SIZE))

#endif /* Cm4Slots_h */
