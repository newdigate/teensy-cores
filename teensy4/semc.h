#ifndef _semc_h_
#define _semc_h_
#ifdef __cplusplus
extern "C" {
#endif
// Bring up the MIMXRT1060-EVKB 32 MB SEMC SDRAM at 0x80000000. FLASHMEM; safe to
// call at early boot (registers only, no Serial/heap/delay()). No-op elsewhere.
void extram_semc_init(void);
#ifdef __cplusplus
}
#endif
#endif
