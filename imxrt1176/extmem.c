/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2019 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// External memory allocation functions.  Attempt to use external memory,
// but automatically fall back to internal RAM if external RAM can't be used.

#include <stdlib.h>
#include "smalloc.h"
#include "wiring.h"

#if defined(__IMXRT1176__)
#define HAS_EXTRAM
// MIMXRT1170-EVKB: 64 MB SEMC SDRAM at 0x80000000..0x83FFFFFF
#define IS_EXTMEM(addr) (((uint32_t)(addr) >> 28) == 8)
// smalloc.h only declares extmem_smalloc_pool for TEENSY41/MIMXRT1060_EVKB; our
// board defines __IMXRT1176__, so declare it here (defined in startup.c). Keeps
// the copied smalloc.h byte-for-byte verbatim.
extern struct smalloc_pool extmem_smalloc_pool;

// _extram_end is the top of the static EXTMEM (.bss.extram) area; the free SDRAM
// above it becomes the pool. (imxrt1176.ld)
extern uint32_t _extram_start;
extern uint32_t _extram_end;

// The 64 MB SDRAM heap is handed to smalloc lazily, on first use, rather than from
// startup.c's ResetHandler.  Calling sm_set_pool() from the .startup boot path drops
// the core into a reset loop before it ever reaches main() -- verified reproducible
// and INDEPENDENT of do_zero / pool size (do_zero=0 fails identically), so it is the
// boot-path CALL, not the pool zeroing.  The RT1176 early boot is extremely layout-
// sensitive: the vector table lives at FLASH+0x2000 in .startup (see startup.c), and
// pulling a cross-module call into that region shifts it (a linker call veneer is the
// likely mechanism; not disassembled to confirm -- cf. the core's FlexRAM/veneer early-
// boot history, where such diagnoses have been wrong before).  Every extmem_* entry
// point below runs only in application context, long after SDRAM is live, so a one-time
// init here is safe; running before the first allocation (including from C++ static
// constructors) preserves "pool ready before use".  do_zero gives calloc semantics via
// per-allocation zeroing in sm_malloc_pool().
static void extmem_ensure_pool(void)
{
	if (extmem_smalloc_pool.pool == 0) {
		sm_set_pool(&extmem_smalloc_pool, &_extram_end,
			64u * 0x100000u - ((uint32_t)&_extram_end - (uint32_t)&_extram_start),
			1, NULL);
	}
}
#elif defined(ARDUINO_TEENSY41) || defined(ARDUINO_MIMXRT1060_EVKB)
#define HAS_EXTRAM
#if defined(ARDUINO_MIMXRT1060_EVKB)
// EVKB external RAM is 32 MB SEMC SDRAM at 0x80000000..0x81FFFFFF
#define IS_EXTMEM(addr) (((uint32_t)(addr) >> 28) == 8)
#else
// Teensy 4.1 external RAM (FlexSPI2 PSRAM) is 0x70000000..0x7FFFFFFF
#define IS_EXTMEM(addr) (((uint32_t)(addr) >> 28) == 7)
#endif
#endif


void *extmem_malloc(size_t size)
{
#ifdef HAS_EXTRAM
#if defined(__IMXRT1176__)
	extmem_ensure_pool();
#endif
	void *ptr = sm_malloc_pool(&extmem_smalloc_pool, size);
	if (ptr) return ptr;
#endif
	return malloc(size);
}

void extmem_free(void *ptr)
{
#ifdef HAS_EXTRAM
	if (IS_EXTMEM(ptr)) {
		sm_free_pool(&extmem_smalloc_pool, ptr);
		return;
	}
#endif
	free(ptr);
}

void *extmem_calloc(size_t nmemb, size_t size)
{
#ifdef HAS_EXTRAM
#if defined(__IMXRT1176__)
	extmem_ensure_pool();
#endif
	// Note: It is assumed that the pool was created with do_zero set to true
	void *ptr = sm_malloc_pool(&extmem_smalloc_pool, nmemb*size);
	if (ptr) return ptr;
#endif
	return calloc(nmemb, size);
}

void *extmem_realloc(void *ptr, size_t size)
{
#ifdef HAS_EXTRAM
#if defined(__IMXRT1176__)
	extmem_ensure_pool();
#endif
	if (IS_EXTMEM(ptr) || ptr == NULL) {
		return sm_realloc_pool(&extmem_smalloc_pool, ptr, size);
	}
#endif
	return realloc(ptr, size);
}
