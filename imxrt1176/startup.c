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
 * ResetHandler configures the FlexRAM ITCM/DTCM bank split (GPR17/16/14) before
 * touching the stack, since _estack now lives at DTCM-top: required on both QEMU
 * (which resizes the TCM RAMBlock on the GPR16/17 writes) and silicon (the
 * BootROM-default DTCM is too small to back DTCM-top). Other RT1062-specific
 * bring-up (DCDC, USB PHY, temp sensor, external RAM, MPU/cache, printf debug)
 * remains removed, to be re-added in proper RT1176 form in later phases.
 * ========================================================================== */

#include <stdint.h>
#include "imxrt1176.h"   /* CCM clock-root, ANADIG PLL/OSC, SysTick, DWT regs */
#include "core_pins.h"   /* NVIC_NUM_INTERRUPTS, IRQ_NUMBER_t, _VectorsRam decl */

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
extern uint32_t _flexram_bank_config;   /* linker absolute symbol; its address IS the value */

extern int main(void);
extern void __libc_init_array(void); /* C++ static constructors            */

/* Core clock. Task 9 brings the ARM PLL up to 996 MHz and routes it to the M7
 * CLOCK_ROOT; set_arm_clock_rt1176() re-affirms this at runtime. Defined here as
 * the single source of truth so timing code (delay.c, Task 10) links against it. */
volatile uint32_t SystemCoreClock = 996000000u;

/* Millisecond tick counter incremented by systick_isr(). DEFINED in delay.c
 * (Task 10); a weak placeholder here lets Task 9 link/run standalone and is
 * silently overridden by delay.c's strong definition once it lands. */
__attribute__((weak)) volatile uint32_t systick_millis_count = 0;

static void memory_copy(uint32_t *dest, const uint32_t *src, uint32_t *dest_end);
static void memory_clear(uint32_t *dest, uint32_t *dest_end);

void ResetHandler(void);
void set_arm_clock_rt1176(void);
void systick_init(void);
void systick_isr(void);
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
	systick_isr,                          /* 15 SysTick (Task 9)             */
	/* small NVIC tail — Phase 0 uses no peripheral interrupts */
	unused_isr, unused_isr, unused_isr, unused_isr,
	unused_isr, unused_isr, unused_isr, unused_isr,
	unused_isr, unused_isr, unused_isr, unused_isr,
	unused_isr, unused_isr, unused_isr, unused_isr,
};

/* ----------------------------------------------------------------------------
 * RAM vector table (DTCM, .vectorsram NOLOAD region). Populated in ResetHandler
 * by copying the 16 architectural + 16 flash NVIC-tail entries and defaulting
 * the remaining external IRQs to unused_isr; VTOR is then repointed here so
 * attachInterruptVector() can install ISRs at run time.
 * -------------------------------------------------------------------------- */
__attribute__((section(".vectorsram"), used, aligned(1024)))
void (* volatile _VectorsRam[16 + NVIC_NUM_INTERRUPTS])(void);

/* ----------------------------------------------------------------------------
 * ResetHandler — boot-ROM entry. Bring RAM up and reach main().
 * -------------------------------------------------------------------------- */
__attribute__((section(".startup"), naked))
void ResetHandler(void)
{
	/* Dual-core hygiene: this is an M7-only image, but the BootROM also releases
	 * the CM4.  With no CM4 firmware it fetches garbage and locks up, which by
	 * default (SRC_SRMR.M4LOCKUP_RESET_MODE=0b00) resets the WHOLE SoC
	 * (SRC_SRSR.M4_LOCKUP_M7, bit12) -> intermittent full boot resets.  Tell SRC
	 * not to reset on a CM4 lockup or CM4 reset-request.  Plain register store,
	 * no stack; must precede anything that a stray CM4 reset would disturb.
	 * (Verified on EVKB: SRMR reads 0x3CC0, SRSR stays clean.) */
	SRC_SRMR |= (0x3u << 6) | (0x3u << 10);   /* M4LOCKUP + M4REQ reset mode = 0b11 (do not reset) */

	/* Configure the FlexRAM ITCM/DTCM bank split BEFORE using the stack: the
	 * BootROM-default split does not back DTCM-top, so this must run first.
	 * ResetHandler is naked and these are plain register stores (no stack use).
	 * RT1176 splits the 16-bank config across GPR17 (low 8 banks) and GPR18
	 * (high 8 banks); GPR16.FLEXRAM_BANK_CFG_SEL (bit2) selects it. Unlike RT1062
	 * there are no GPR14 TCM-size or GPR16 INIT_ITCM/DTCM_EN fields to write. */
	IOMUXC_GPR_GPR17 = ((uint32_t)&_flexram_bank_config) & 0xFFFFu;         /* banks 0-7  */
	IOMUXC_GPR_GPR18 = (((uint32_t)&_flexram_bank_config) >> 16) & 0xFFFFu; /* banks 8-15 */
	IOMUXC_GPR_GPR16 |= 0x4u;                                               /* FLEXRAM_BANK_CFG_SEL */
	__asm__ volatile("dsb":::"memory"); __asm__ volatile("isb":::"memory");

	/* The boot-ROM already loaded MSP from the vector table, but set it
	 * again explicitly so we are independent of how we were entered. */
	__asm__ volatile("mov sp, %0" : : "r" ((uint32_t)&_estack) : "memory");

	/* enable the FPU (CP10/CP11 full access) */
	*(volatile uint32_t *)0xE000ED88 |= (0xF << 20);
	__asm__ volatile("dsb":::"memory");
	__asm__ volatile("isb":::"memory");

	/* copy fast code from flash LMA to ITCM, and .data from flash to DTCM */
	memory_copy(&_stext, &_stextload, &_etext);
	memory_copy(&_sdata, &_sdataload, &_edata);

	/* zero .bss */
	memory_clear(&_sbss, &_ebss);

	/* Build the RAM vector table: copy the 16 architectural + 16 flash NVIC-tail
	 * entries, default the remaining external IRQs to unused_isr, then repoint
	 * VTOR at RAM so attachInterruptVector() can install ISRs at run time.
	 * (_VectorsFlash is [16 + 16]; only indices < 32 are read from it.) */
	for (unsigned i = 0; i < 16 + NVIC_NUM_INTERRUPTS; i++) {
		_VectorsRam[i] = (i < 16 + 16) ? _VectorsFlash[i] : unused_isr;
	}
	__asm__ volatile("dsb":::"memory");
	SCB_VTOR = (uint32_t)_VectorsRam;
	__asm__ volatile("dsb":::"memory"); __asm__ volatile("isb":::"memory");

	/* Raise the CM7 core supply (DCDC VDD1P0) to 1.15 V OverDrive BEFORE boosting to
	 * 996 MHz.  Mirrors the SDK BOARD_BootClockRUN: put the DCDC in CCM mode, clear
	 * REG3.VDD1P0CTRL_DISABLE_STEP (a bare CTRL1 write with stepping disabled does
	 * NOTHING - why the first attempt failed), set the buck target, then wait for
	 * REG0.STS_DC_OK.  DCDC @ 0x40CA8000: CTRL1@+4, REG0@+8, REG2@+0x10, REG3@+0x14.
	 * VDD1P0CTRL_TRG[12:8]: 16=1.0 V, +1 step=+25 mV, 22=1.15 V.  Waits are bounded. */
	{
		volatile uint32_t *DCDC_CTRL1 = (volatile uint32_t *)0x40CA8004u;
		volatile uint32_t *DCDC_REG0  = (volatile uint32_t *)0x40CA8008u;
		volatile uint32_t *DCDC_REG2  = (volatile uint32_t *)0x40CA8010u;
		volatile uint32_t *DCDC_REG3  = (volatile uint32_t *)0x40CA8014u;
		uint32_t g;
		*DCDC_REG0 = (*DCDC_REG0 & ~0x04000000u) | 0x1u;         /* CCM: -PWD_CMP_OFFSET +PWD_ZCD */
		*DCDC_REG2 = (*DCDC_REG2 & ~0x00000E00u) | (3u << 9);    /* LOOPCTRL_EN_RCSCALE = 3       */
		*DCDC_REG3 &= ~0x20000000u;                              /* enable VDD1P0 stepping        */
		*DCDC_CTRL1 = (*DCDC_CTRL1 & ~0x1F00u) | (22u << 8);     /* VDD1P0CTRL_TRG = 22 (1.15 V)  */
		for (g = 0; g < 200000u && !(*DCDC_REG0 & 0x80000000u); g++) { }  /* wait STS_DC_OK */
		__asm__ volatile("dsb":::"memory");
	}

	/* RT1176 CCM bring-up: ARM PLL -> 996 MHz on the M7 core CLOCK_ROOT */
	set_arm_clock_rt1176();

	/* 1 kHz SysTick + DWT cycle counter (must follow the clock init so the
	 * reload value matches the now-996 MHz core) */
	systick_init();

	/* C++ static constructors, then the application */
	__libc_init_array();
	main();

	while (1) { __asm__ volatile("wfi"); }
}

/* ----------------------------------------------------------------------------
 * set_arm_clock_rt1176 — RT1176 CCM bring-up for a 996 MHz Cortex-M7 core.
 *
 * Faithful (but minimal) port of the EVKB SDK BOARD_BootClockRUN()
 *   (_boards/evkbmimxrt1170/.../clock_config.c) translated to direct register
 *   writes through imxrt1176.h. We bring up exactly the path Phase 0 needs:
 *     - confirm the 24 MHz crystal OSC is enabled/stable (PLL reference),
 *     - park the M7 core root on a safe slow source while the PLL re-locks,
 *     - configure + lock the ARM PLL (mirrors CLOCK_InitArmPll):
 *         Fout = Fin * (loopDivider / (2 * postDivider))
 *              = 24 MHz * (166 / (2*2)) = 996 MHz
 *       (loopDivider 166, postDivider kCLOCK_PllPostDiv2 == encoded 0),
 *     - route the ARM PLL to the M7 core CLOCK_ROOT (root 0, mux 4, div 1),
 *     - point the AHB/bus CLOCK_ROOT (root 2) at a safe source.
 *   GPIO/IOMUXC roots used by Phase 0 are left on their boot-ROM defaults
 *   (24 MHz OSC-derived); they are not on a CCM root that needs re-muxing for
 *   correctness here.
 *
 * QEMU caveat: QEMU's CCM is a clock-survival model — most of these writes are
 * absorbed and the CPU clock is wired to 996 MHz at the board level. Every
 * poll loop below is bounded (guard counters) so it cannot hang when a status
 * bit QEMU does not model never sets. On real silicon the loops spin until the
 * hardware reports OSC-stable / PLL-stable (hardware validation is Task 13).
 * -------------------------------------------------------------------------- */
void set_arm_clock_rt1176(void)
{
	volatile int i;

	/* --- 1. 24 MHz crystal OSC (ARM PLL reference) --------------------- */
	ANADIG_OSC_OSC_24M_CTRL |= ANADIG_OSC_OSC_24M_CTRL_OSC_EN |
	                           ANADIG_OSC_OSC_24M_CTRL_LP_EN;
	for (i = 0; i < 100000 &&
	     !(ANADIG_OSC_OSC_24M_CTRL & ANADIG_OSC_OSC_24M_CTRL_OSC_24M_STABLE); i++) {
		/* bounded wait for OSC stable */
	}

	/* --- 2. Park M7 core root on a slow source during PLL re-lock ------ */
	/* CLOCK_SetRootClock(kCLOCK_Root_M7, mux=OscRc48MDiv2(0), div=1) */
	CCM_CLOCK_ROOT0_CONTROL = CCM_CLOCK_ROOT_CONTROL_MUX(0) |
	                          CCM_CLOCK_ROOT_CONTROL_DIV(0);
	__asm__ volatile("dsb":::"memory");
	__asm__ volatile("isb":::"memory");

	/* --- 3. ARM PLL -> 996 MHz (mirrors CLOCK_InitArmPll) -------------- */
	{
		uint32_t reg = ANADIG_PLL_ARM_PLL_CTRL;

		/* Power the PLL down before reconfiguring (clear POWERUP/ENABLE,
		 * set GATE) so the loop/post dividers can change cleanly. */
		reg &= ~(ANADIG_PLL_ARM_PLL_CTRL_POWERUP |
		         ANADIG_PLL_ARM_PLL_CTRL_ENABLE_CLK |
		         ANADIG_PLL_ARM_PLL_CTRL_ARM_PLL_STABLE);
		reg |= ANADIG_PLL_ARM_PLL_CTRL_ARM_PLL_GATE;
		ANADIG_PLL_ARM_PLL_CTRL = reg;

		/* Program dividers, power up, hold ring off, keep gated for now. */
		reg &= ~(ANADIG_PLL_ARM_PLL_CTRL_DIV_SELECT(0xFF) |
		         ANADIG_PLL_ARM_PLL_CTRL_POST_DIV_SEL(0x3));
		reg |= ANADIG_PLL_ARM_PLL_CTRL_DIV_SELECT(166) |   /* loopDivider 166 */
		       ANADIG_PLL_ARM_PLL_CTRL_POST_DIV_SEL(0) |    /* PllPostDiv2 -> 996 MHz */
		       ANADIG_PLL_ARM_PLL_CTRL_ARM_PLL_GATE |
		       ANADIG_PLL_ARM_PLL_CTRL_POWERUP |
		       ANADIG_PLL_ARM_PLL_CTRL_HOLD_RING_OFF;
		ANADIG_PLL_ARM_PLL_CTRL = reg;
		__asm__ volatile("dsb":::"memory");
		__asm__ volatile("isb":::"memory");

		/* Settling delay (SDK waits ~30us); a short spin suffices here. */
		for (i = 0; i < 3000; i++) { /* ~PLL ramp */ }

		/* Release the hold-ring and wait (bounded) for PLL stable. */
		reg &= ~ANADIG_PLL_ARM_PLL_CTRL_HOLD_RING_OFF;
		ANADIG_PLL_ARM_PLL_CTRL = reg;
		for (i = 0; i < 100000 &&
		     !(ANADIG_PLL_ARM_PLL_CTRL & ANADIG_PLL_ARM_PLL_CTRL_ARM_PLL_STABLE); i++) {
			/* bounded wait for PLL stable */
		}

		/* Enable the clock output and ungate. */
		reg |= ANADIG_PLL_ARM_PLL_CTRL_ENABLE_CLK;
		reg &= ~ANADIG_PLL_ARM_PLL_CTRL_ARM_PLL_GATE;
		ANADIG_PLL_ARM_PLL_CTRL = reg;
		__asm__ volatile("dsb":::"memory");
		__asm__ volatile("isb":::"memory");
	}

	/* --- 4. Route ARM PLL to the M7 core CLOCK_ROOT (996 MHz) ---------- */
	/* CLOCK_SetRootClock(kCLOCK_Root_M7, mux=MuxArmPllOut(4), div=1) */
	CCM_CLOCK_ROOT0_CONTROL = CCM_CLOCK_ROOT_CONTROL_MUX(4) |
	                          CCM_CLOCK_ROOT_CONTROL_DIV(0);
	__asm__ volatile("dsb":::"memory");
	__asm__ volatile("isb":::"memory");

	/* --- 5. AHB/bus CLOCK_ROOT — keep on a safe OSC-derived source ----- */
	/* The SDK routes BUS off SYS_PLL3 (not brought up in this Phase-0 subset);
	 * leave the bus root on its boot-ROM-safe slow source (mux 0, div 1). */
	CCM_CLOCK_ROOT2_CONTROL = CCM_CLOCK_ROOT_CONTROL_MUX(0) |
	                          CCM_CLOCK_ROOT_CONTROL_DIV(0);
	__asm__ volatile("dsb":::"memory");
	__asm__ volatile("isb":::"memory");

	SystemCoreClock = 996000000u;
}

/* ----------------------------------------------------------------------------
 * systick_init — 1 kHz SysTick from the (now 996 MHz) core clock, plus the DWT
 * cycle counter (used by the Teensy cycle-accurate delay primitives).
 * -------------------------------------------------------------------------- */
void systick_init(void)
{
	SYST_RVR = (996000000u / 1000u) - 1u;   /* 1 ms reload @ 996 MHz */
	SYST_CVR = 0;
	SYST_CSR = 7;                            /* ENABLE | TICKINT | CLKSOURCE */

	/* DWT cycle counter for sub-microsecond timing. */
	ARM_DEMCR |= ARM_DEMCR_TRCENA;
	ARM_DWT_CTRL |= 1u;                      /* CYCCNTENA */
}

/* ----------------------------------------------------------------------------
 * systick_isr — 1 kHz tick. Increments the millisecond counter that delay.c
 * (Task 10) exposes via millis(). Kept in flash with the rest of startup.
 * -------------------------------------------------------------------------- */
void systick_isr(void)
{
	systick_millis_count++;
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

/* Install an ISR into the RAM vector table (external IRQs start at index 16). */
void attachInterruptVector(IRQ_NUMBER_t irq, void (*function)(void)) {
	_VectorsRam[16 + (int)irq] = function;
	__asm__ volatile("dsb":::"memory"); __asm__ volatile("isb":::"memory");
}

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
