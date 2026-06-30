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

/* ============================================================================
 * MIMXRT1170-EVKB (i.MX RT1176, Cortex-M7) — minimal Phase-0 ResetHandler.
 *
 * This is the *first* end-to-end boot bring-up for the EVKB port. Its only job
 * is to get from the QEMU/silicon FlexSPI boot-ROM hand-off to main():
 *   - boot-ROM validates the FCB/IVT (see bootdata.c) and branches to the
 *     entry it reads from IVT.entry (== ResetHandler); it also loads the
 *     initial MSP + VTOR from the Cortex-M vector table at flash base + 0x2000.
 *   - the linker (imxrt1176.ld) places .startup first in .text.code at
 *     ORIGIN(FLASH)+0x2000 == 0x30002000, so the vector table below lands
 *     exactly where the boot-ROM expects it.
 *
 * It relies on the clocks the boot-ROM already set up (real CCM config is a
 * later task), does the .text-to-ITCM / .data copy and .bss zero that the
 * linker LMA->VMA layout requires, enables the FPU, runs C++ constructors,
 * and calls main().
 *
 * QEMU models ITCM and DTCM as full RAM regions regardless of FlexRAM config,
 * so no FlexRAM bank configuration is needed to boot here. All RT1062-specific
 * bring-up (PLL/CCM, FlexRAM banks, DCDC, USB PHY, temp sensor, external RAM,
 * MPU/cache, printf debug) has been removed and will be re-added in proper
 * RT1176 form in later phases.
 * ========================================================================== */

#include <stdint.h>

#define SCB_VTOR (*(volatile uint32_t *)0xE000ED08u)

/* ---- symbols exported by imxrt1176.ld ---- */
extern uint32_t _stext;       /* ITCM .text.itcm VMA start                  */
extern uint32_t _etext;       /* ITCM .text.itcm VMA end (incl .ARM.exidx)  */
extern uint32_t _stextload;   /* ITCM .text.itcm LMA (in FLASH)             */
extern uint32_t _sdata;       /* DTCM .data VMA start                       */
extern uint32_t _edata;       /* DTCM .data VMA end                         */
extern uint32_t _sdataload;   /* DTCM .data LMA (in FLASH)                  */
extern uint32_t _sbss;        /* DTCM .bss VMA start                        */
extern uint32_t _ebss;        /* DTCM .bss VMA end                          */
extern uint32_t _estack;      /* top of stack (DTCM)                        */

extern int main(void);
extern void __libc_init_array(void); /* C++ static constructors            */

/* Provisional core clock. The QEMU boot-ROM leaves the part at its default
 * clock tree; the real CCM bring-up (and a correct SystemCoreClock) is Task 9.
 * Declared/defined here so timing code that links against it has a symbol. */
volatile uint32_t SystemCoreClock = 528000000u;

static void memory_copy(uint32_t *dest, const uint32_t *src, uint32_t *dest_end);
static void memory_clear(uint32_t *dest, uint32_t *dest_end);

void ResetHandler(void);
static void fault_isr(void);
static void unused_isr(void);

/* ----------------------------------------------------------------------------
 * Cortex-M vector table (flash, at 0x30002000 via .startup placement).
 *
 * entry[0] = initial MSP (_estack), entry[1] = ResetHandler.
 * The 16 architectural exceptions are populated; everything else (a small
 * NVIC tail sufficient for the boot-ROM's MSP/VTOR fetch) routes to
 * unused_isr. Phase-0 EVKB needs no peripheral ISRs; SysTick is added in
 * Task 9. VTOR is repointed here in ResetHandler.
 * -------------------------------------------------------------------------- */
__attribute__((section(".vectors"), used))
void (* const _VectorsFlash[16 + 16])(void) =
{
	(void (*)(void))((uint32_t)&_estack), /*  0 Initial stack pointer        */
	ResetHandler,                         /*  1 Reset                        */
	fault_isr,                            /*  2 NMI                          */
	fault_isr,                            /*  3 HardFault                    */
	fault_isr,                            /*  4 MemManage                    */
	fault_isr,                            /*  5 BusFault                     */
	fault_isr,                            /*  6 UsageFault                   */
	unused_isr,                           /*  7 reserved                     */
	unused_isr,                           /*  8 reserved                     */
	unused_isr,                           /*  9 reserved                     */
	unused_isr,                           /* 10 reserved                     */
	unused_isr,                           /* 11 SVCall                       */
	unused_isr,                           /* 12 DebugMonitor                 */
	unused_isr,                           /* 13 reserved                     */
	unused_isr,                           /* 14 PendSV                       */
	unused_isr,                           /* 15 SysTick                      */
	/* small NVIC tail — Phase 0 uses no peripheral interrupts */
	unused_isr, unused_isr, unused_isr, unused_isr,
	unused_isr, unused_isr, unused_isr, unused_isr,
	unused_isr, unused_isr, unused_isr, unused_isr,
	unused_isr, unused_isr, unused_isr, unused_isr,
};

/* ----------------------------------------------------------------------------
 * ResetHandler — boot-ROM entry. Bring RAM up and reach main().
 * -------------------------------------------------------------------------- */
__attribute__((section(".startup"), naked))
void ResetHandler(void)
{
	/* The boot-ROM already loaded MSP from the vector table, but set it
	 * again explicitly so we are independent of how we were entered. */
	__asm__ volatile("mov sp, %0" : : "r" ((uint32_t)&_estack) : "memory");

	/* point VTOR at our flash vector table */
	SCB_VTOR = (uint32_t)_VectorsFlash;

	/* enable the FPU (CP10/CP11 full access) */
	*(volatile uint32_t *)0xE000ED88 |= (0xF << 20);
	__asm__ volatile("dsb":::"memory");
	__asm__ volatile("isb":::"memory");

	/* copy fast code from flash LMA to ITCM, and .data from flash to DTCM */
	memory_copy(&_stext, &_stextload, &_etext);
	memory_copy(&_sdata, &_sdataload, &_edata);

	/* zero .bss */
	memory_clear(&_sbss, &_ebss);

	/* provisional clock (real CCM is Task 9) */
	SystemCoreClock = 528000000u;

	/* C++ static constructors, then the application */
	__libc_init_array();
	main();

	while (1) { __asm__ volatile("wfi"); }
}

/* ----------------------------------------------------------------------------
 * Default exception/interrupt handlers — spin so faults are observable.
 * -------------------------------------------------------------------------- */
__attribute__((section(".startup")))
static void fault_isr(void)
{
	while (1) { /* hang */ }
}

__attribute__((section(".startup")))
static void unused_isr(void)
{
	while (1) { /* hang */ }
}

/* ----------------------------------------------------------------------------
 * Minimal memory copy / clear (kept in .startup, runs from flash before ITCM
 * is populated).
 * -------------------------------------------------------------------------- */
__attribute__((section(".startup"), noinline))
static void memory_copy(uint32_t *dest, const uint32_t *src, uint32_t *dest_end)
{
	if (dest == src) return;
	while (dest < dest_end) {
		*dest++ = *src++;
	}
}

__attribute__((section(".startup"), noinline))
static void memory_clear(uint32_t *dest, uint32_t *dest_end)
{
	while (dest < dest_end) {
		*dest++ = 0;
	}
}

/* ----------------------------------------------------------------------------
 * Weak stubs for teensy startup symbols that other (not-yet-ported) core files
 * may reference. Kept minimal — they do nothing. Real implementations arrive in
 * later phases.
 * -------------------------------------------------------------------------- */
__attribute__((weak)) void startup_early_hook(void) {}
__attribute__((weak)) void startup_middle_hook(void) {}
__attribute__((weak)) void startup_late_hook(void) {}
__attribute__((weak)) void unused_interrupt_vector(void) { while (1) {} }

/* __libc_init_array() walks .init_array then calls _init(); we don't use the
 * .init/.fini crt objects, so provide empty stubs. */
__attribute__((weak)) void _init(void) {}
__attribute__((weak)) void _fini(void) {}

/* ----------------------------------------------------------------------------
 * Minimal newlib syscall stubs (kept in this file with the entry point so the
 * linker does not discard them).
 * -------------------------------------------------------------------------- */
#include <errno.h>
#include <sys/stat.h>

extern uint32_t _heap_start;
extern uint32_t _heap_end;
char *__brkval = (char *)&_heap_start;

__attribute__((weak))
void * _sbrk(int incr)
{
	char *prev = __brkval;
	if (incr != 0) {
		if (prev + incr > (char *)&_heap_end) {
			errno = ENOMEM;
			return (void *)-1;
		}
		__brkval = prev + incr;
	}
	return prev;
}

__attribute__((weak))
int _read(int file __attribute__((unused)), char *ptr __attribute__((unused)), int len __attribute__((unused)))
{
	return 0;
}

__attribute__((weak))
int _write(int file __attribute__((unused)), char *ptr __attribute__((unused)), int len)
{
	return len;
}

__attribute__((weak))
int _close(int fd __attribute__((unused)))
{
	return -1;
}

__attribute__((weak))
int _fstat(int fd __attribute__((unused)), struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

__attribute__((weak))
int _isatty(int fd __attribute__((unused)))
{
	return 1;
}

__attribute__((weak))
int _lseek(int fd __attribute__((unused)), long long offset __attribute__((unused)), int whence __attribute__((unused)))
{
	return -1;
}

__attribute__((weak))
void _exit(int status __attribute__((unused)))
{
	while (1) { __asm__ volatile("wfi"); }
}

__attribute__((weak))
void __cxa_pure_virtual(void)
{
	while (1) { __asm__ volatile("wfi"); }
}

__attribute__((weak))
int __cxa_guard_acquire(char *g)
{
	return !(*g);
}

__attribute__((weak))
void __cxa_guard_release(char *g)
{
	*g = 1;
}

__attribute__((weak))
void abort(void)
{
	while (1) { __asm__ volatile("wfi"); }
}
