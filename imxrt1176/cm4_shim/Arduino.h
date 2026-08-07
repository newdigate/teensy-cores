// Arduino-lite for the freestanding CM4 image world. This is NOT the CM7
// Arduino.h: it provides the minimal surface AudioStream + gate nodes need.
// Grow it deliberately; anything with hardware behind it must come from a
// shared-core library or a probe-backed sequence, never from CM7 headers.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>   // freestanding prototypes; impls from runtime_stubs.c
#include <stdlib.h>   // abs()/labs() — mirrors the real core (wiring.h:35). Node
                      // code (e.g. analyze_peak.h read()) uses Arduino's abs();
                      // GCC lowers abs(int) to __builtin_abs, so this stays a
                      // pure declaration with no -nostdlib link dependency.

#define EVKB_CM4_WORLD 1

// --- OPT-IN: the SoC register map --------------------------------------------
//
// OFF BY DEFAULT. An image asks for it with -DEVKB_CM4_SOC_REGS (the macro's
// DEFINES list). README.md's never-list says "no blind copies of imxrt1176.h
// content" and "never from CM7 headers" -- that rule stands for every image
// that does not name this flag, and the flag is greppable so the exceptions
// are countable.
//
// Why the exception exists. Up to Phase 7.1 every CM4 gate drove peripherals
// from a handful of probe-backed literals in its own main, which is fine for a
// ~100-line register sequence. Phase 7.2 compiles a whole Arduino LIBRARY --
// USBHost_t36 -- and that library reaches the USB block through the Teensy-idiom
// macros the real Arduino.h supplies: utility/imxrt_usbhs.h is nothing but
// USBHS_* -> USB2_* aliases, and ehci.cpp's RT1176 branch names USBPHY2_*,
// USBPHY_CTRL_SFTRST, USBPHY_PLL_SIC_* and CCM_LPCG115_DIRECT directly.
// Re-spelling ~70 registers and bit masks as CM4-local literals would create a
// second source of truth for exactly the addresses that must not drift -- the
// duplication the 3.3 shared-core consolidation exists to end.
//
// What makes it defensible: imxrt1176.h is #defines plus two no-op cache
// inlines. It emits no code, encodes no ownership, and peripheral MMIO answers
// at the same system addresses from either core (RM Table 3-1 vs 3-2). It is a
// memory map, not a driver.
//
// ★ And the never-list was NOT paranoia -- the four #undefs below are it being
// right. Read them before assuming the rest of the header is inert.
//
// TCM is the one place the two cores' views genuinely differ, and nothing here
// relies on that: see the DMAMEM note at the foot of this file.
#if defined(EVKB_CM4_SOC_REGS)
#include "imxrt1176.h"

// imxrt1176.h defines the CM7's DMAMEM (".dmabuffers", gathered into .bss.dma
// by imxrt1176.ld). The CM4 image world names its OUTPUT section directly and
// links it somewhere else entirely, so drop the CM7 spelling here rather than
// let the redefinition below decide it by ordering. See the DMAMEM block at the
// foot of this file for why the section differs.
#undef DMAMEM

// ★ The NVIC accessors below are this world's, not the core's. imxrt1176.h
// spells them as function-like MACROS (NVIC_ENABLE_IRQ(n) etc.); the shim
// spells them as inline FUNCTIONS, and a macro of the same name mangles the
// function's own definition into nonsense at the point of declaration --
// dozens of "expected ')' before 'volatile'" errors pointing at the CORE
// header, which is a thoroughly misleading place to start debugging.
// Same registers, same arithmetic: this picks a spelling, it does not change
// behaviour, and dropping the core's keeps existing CM4 images byte-identical.
#undef NVIC_ENABLE_IRQ
#undef NVIC_DISABLE_IRQ
#undef NVIC_SET_PENDING
#undef NVIC_SET_PRIORITY

// External interrupt numbers this world needs BY NAME. The CM7 gets them from
// core_pins.h, which is the pin/GPIO API and unusable here. 135 = USB OTG2 in
// RM Table 4-2 (the CM4 domain interrupt summary) -- the line Phase 7.1
// HW-proved reaches the CM4's own NVIC. These literals must stay identical to
// core_pins.h:70/74; a divergence would put the handler in the wrong vector
// slot, which shows up as silence, not as an error.
#define IRQ_USB_OTG2 135
#define IRQ_USB2     IRQ_USB_OTG2   // the name utility/imxrt_usbhs.h expects
#endif // EVKB_CM4_SOC_REGS

// --- interrupt control (CM4 = same ARMv7-M primitives) ---
static inline void __disable_irq(void) { __asm volatile ("cpsid i" ::: "memory"); }
static inline void __enable_irq(void)  { __asm volatile ("cpsie i" ::: "memory"); }

// --- NVIC, CM4 world: raw registers, static vector table (no RAM vectors) ---
typedef int IRQ_NUMBER_t;
#define NVIC_ISER_BASE 0xE000E100u
#define NVIC_ICER_BASE 0xE000E180u
#define NVIC_ISPR_BASE 0xE000E200u
#define NVIC_IPRI_BASE 0xE000E400u
static inline void NVIC_ENABLE_IRQ(int n)  { *(volatile uint32_t *)(NVIC_ISER_BASE + 4u*((uint32_t)n >> 5)) = 1u << (n & 31); }
static inline void NVIC_DISABLE_IRQ(int n) { *(volatile uint32_t *)(NVIC_ICER_BASE + 4u*((uint32_t)n >> 5)) = 1u << (n & 31); }
static inline void NVIC_SET_PENDING(int n) { *(volatile uint32_t *)(NVIC_ISPR_BASE + 4u*((uint32_t)n >> 5)) = 1u << (n & 31); }
static inline void NVIC_SET_PRIORITY(int n, int p) { *(volatile uint8_t *)(NVIC_IPRI_BASE + (uint32_t)n) = (uint8_t)p; }

// The CM4 vector table is static (startup_cm4.S). attachInterruptVector is a
// no-op here: the handler symbol must already be in the table. AudioStream's
// software_isr is aliased to the table slot by the gate's startup file.
typedef void (*voidFuncPtr)(void);
static inline void attachInterruptVector(IRQ_NUMBER_t irq, voidFuncPtr f) { (void)irq; (void)f; }

// --- DWT cycle counter (Cortex-M4 has DWT/CYCCNT; enable in gate init) ---
#define ARM_DWT_CYCCNT (*(volatile uint32_t *)0xE0001004u)

// IRQ_SOFTWARE: same repurposed-CAN1 slot 44 as the CM7 world (CAN1 unused by
// repo convention on BOTH cores — FlexCAN work uses CAN3). AudioStream.h's
// #ifndef guard picks this up.
#define IRQ_SOFTWARE ((IRQ_NUMBER_t)44)

// F_CPU_ACTUAL — declaration only, mirroring core_pins.h:2919: AudioStream.h's
// in-class inline processorUsage()/processorUsageMax() parse
// CYCLE_COUNTER_APPROX_PERCENT, which names it. Nothing here DEFINES it: an
// image that odr-uses the CPU-usage-percent API fails at link instead of
// silently computing with a fake CM4 clock. (See cm4_shim/README.md log.)
extern volatile uint32_t F_CPU_ACTUAL;

// --- time base: millis / micros / delay / delayMicroseconds ------------------
//
// All four are derived from ARM_DWT_CYCCNT. ★ Do NOT reintroduce a counted spin
// loop. cm4_usb_irq_probe carried one with a "~3 cycles/iter" estimate that was
// really nearer 7 (a volatile decrement is LDR+SUBS+STR+CMP+B), so a nominal
// 8 s window ran ~18.7 s, collided with the CM7's 20 s receive timeout, and
// truncated the transcript in a way that read as "the CM4 hung" rather than
// "the delay was miscalibrated". DWT measures; a loop guesses.
//
// PRECONDITION: the image must enable the cycle counter (DEMCR.TRCENA, then
// DWT_CTRL.CYCCNTENA) before the first call, exactly as the gates' dwt_start()
// does. Without it CYCCNT stays 0, every clock below stands still, and for
// USBHost_t36 that means no timeout ever fires — a silent failure, so enable it
// first thing in the image's init.
//
// The CM4 core clock is 400 MHz; the CM7's is 996 MHz. F_CPU_ACTUAL above is
// the CM7's value and is deliberately never DEFINED here, so it must not be
// used for CM4 timing — hence a separate, differently named constant that
// cannot be mistaken for it at a call site.
#define F_CPU_CM4         400000000u
#define CM4_CYCLES_PER_US (F_CPU_CM4 / 1000000u)   /* 400, exact */

// CYCCNT is 32-bit, so at 400 MHz it wraps every 2^32/400e6 = 10.74 s. millis()
// and micros() therefore cannot divide the raw counter — each call folds the
// elapsed-cycle delta (wrap-correct in unsigned 32-bit arithmetic) into
// software counters. cm4_dwt_last advances only by WHOLE microseconds, so the
// sub-microsecond remainder stays in the hardware counter and the clocks do not
// drift. micros() then wraps at 2^32 us (~71.6 min) and millis() independently
// at 2^32 ms (~49.7 days), both matching Arduino — so the usual unsigned
// (now - then) comparison stays correct across a wrap.
//
// LIMIT: only ONE wrap can be inferred per call, so something must call one of
// these at least every ~10.7 s or whole wraps are lost. delay() below polls
// millis(), so long delays are safe; an otherwise idle image must poll.
//
// The state is __attribute__((weak)) at file scope rather than function-local
// statics: static-inline-with-statics would give EVERY translation unit its own
// epoch, so a millis() stamped in one USBHost_t36 .cpp and compared in another
// would be measured against a different zero, and any TU quiet for >10.7 s
// would silently lose wraps. Weak definitions collapse to one object at link
// time, identically in C and C++.
__attribute__((weak)) volatile uint32_t cm4_dwt_last;   /* CYCCNT at last fold  */
__attribute__((weak)) volatile uint32_t cm4_us;         /* us since boot        */
__attribute__((weak)) volatile uint32_t cm4_ms;         /* ms since boot        */
__attribute__((weak)) volatile uint32_t cm4_us_frac;    /* us not yet folded into ms, 0..999 */

// PRIMASK save/restore, not a bare cpsid/cpsie pair: USBHost_t36 calls millis()
// from its ISR as well as from main context, so the fold must be atomic without
// re-enabling interrupts in a caller that had deliberately masked them.
static inline uint32_t cm4_irq_save(void) {
    uint32_t pri;
    __asm volatile ("mrs %0, primask" : "=r" (pri) :: "memory");
    __asm volatile ("cpsid i" ::: "memory");
    return pri;
}
static inline void cm4_irq_restore(uint32_t pri) {
    if (!pri) __asm volatile ("cpsie i" ::: "memory");
}

/* Fold elapsed cycles into the software clocks. Caller holds the mask.
 * dus <= (2^32-1)/400 = 10737418, so dus*400 <= 4294967200 cannot overflow,
 * and dus+cm4_us_frac stays well inside uint32 too. Every division is by a
 * 32-bit constant, which GCC lowers to a multiply-high — no __aeabi_uldivmod,
 * which matters because the image links -nostdlib (no libgcc). */
static inline void cm4_time_fold(void) {
    uint32_t dus  = (uint32_t)(ARM_DWT_CYCCNT - cm4_dwt_last) / CM4_CYCLES_PER_US;
    cm4_dwt_last += dus * CM4_CYCLES_PER_US;   /* whole us only; remainder stays */
    cm4_us       += dus;
    dus          += cm4_us_frac;
    cm4_ms       += dus / 1000u;
    cm4_us_frac   = dus % 1000u;
}

static inline uint32_t micros(void) {
    uint32_t pri = cm4_irq_save();
    cm4_time_fold();
    uint32_t v = cm4_us;
    cm4_irq_restore(pri);
    return v;
}
static inline uint32_t millis(void) {
    uint32_t pri = cm4_irq_save();
    cm4_time_fold();
    uint32_t v = cm4_ms;
    cm4_irq_restore(pri);
    return v;
}

/* Bounded wait on the same time base — never a counted loop. Polling millis()
 * also keeps the fold above current, so a long delay cannot lose a wrap. */
static inline void delay(uint32_t ms) {
    uint32_t start = millis();
    while ((uint32_t)(millis() - start) < ms) { }
}
/* us * 400 must fit in uint32, i.e. us <= 10.7e6; past that use delay(). Reads
 * CYCCNT directly (no fold) because it is short by construction. */
static inline void delayMicroseconds(uint32_t us) {
    uint32_t start  = ARM_DWT_CYCCNT;
    uint32_t cycles = us * CM4_CYCLES_PER_US;
    while ((uint32_t)(ARM_DWT_CYCCNT - start) < cycles) { }
}

// --- OPT-IN: Arduino C++ types a LIBRARY HEADER needs to PARSE ---------------
//
// OFF BY DEFAULT. An image asks for it with -DEVKB_CM4_ARDUINO_CXX.
// README.md's never-list says "no Print/Stream/String", and that rule stands
// for every image that does not name this flag.
//
// ★ Read what this is before reusing it: it is a PARSE fix, not a port.
// Nothing below is ever called, instantiated, or linked by a CM4 image. The
// types exist because USBHost_t36.h declares its whole driver family in one
// header, and three of those declarations need complete types even when the
// corresponding .cpp is not compiled:
//
//   USBHost_t36.h:1189   elapsedMicros em_sw_;              (a MEMBER: needs a size)
//   USBHost_t36.h:1595   class USBSerialBase : ... Stream   (a BASE: needs a definition)
//   USBHost_t36.h:2014   class USBSerialEmu  : ... Stream
//
// Neither serial class's vtable is emitted -- their virtuals are defined in
// serial.cpp and SerEMU.cpp, which a CM4 image does not build -- so this costs
// nothing at link time and cannot make a CM4 image print.
//
// The moment anyone wants a WORKING Print on the CM4, this is the wrong file:
// take it to the coordinator. No UART belongs to the CM4 in this tree;
// observations leave over the MU mailbox.
#if defined(EVKB_CM4_ARDUINO_CXX) && defined(__cplusplus)

// Print/Stream, deliberately ABSTRACT so they cannot be instantiated even by
// accident, and deliberately tiny. `using Print::write;` in both serial classes
// is the only thing that constrains the shape: it needs the two write overloads
// to name something.
class Print {
public:
    virtual size_t write(uint8_t) = 0;
    virtual size_t write(const uint8_t *buffer, size_t size) = 0;
};
class Stream : public Print {
public:
    virtual int available() = 0;
    virtual int read() = 0;
    virtual int peek() = 0;
};

// elapsedMillis/elapsedMicros: the same shape as the core's elapsedMillis.h
// (PJRC, MIT -- the file this tree already carries), reduced to the operations
// a complete type needs. Nothing in the CM4 world constructs or reads one; the
// type exists so USBHost_t36.h's `elapsedMicros em_sw_` member has a size.
//
// ★ The core header is NOT #included, and the reason is a trap worth naming:
// elapsedMillis.h lives in cores/imxrt1176/ and opens with a QUOTED
// `#include "Arduino.h"`. A quoted include searches the including FILE's own
// directory first, ahead of every -I, so from there "Arduino.h" resolves to the
// CM7 core header -- which drags in core_pins.h and collides head-on with this
// shim (IRQ_NUMBER_t re-declared as an enum, attachInterruptVector/__enable_irq
// redeclared with C linkage). The INCLUDE_DIRS ordering that makes the shim win
// for <Arduino.h> cannot help: it is out-ranked by the includer's directory.
class elapsedMillis {
private:
    unsigned long ms;
public:
    elapsedMillis(void) { ms = millis(); }
    elapsedMillis(unsigned long val) { ms = millis() - val; }
    operator unsigned long () const { return millis() - ms; }
    elapsedMillis & operator = (unsigned long val) { ms = millis() - val; return *this; }
};
class elapsedMicros {
private:
    unsigned long us;
public:
    elapsedMicros(void) { us = micros(); }
    elapsedMicros(unsigned long val) { us = micros() - val; }
    operator unsigned long () const { return micros() - us; }
    elapsedMicros & operator = (unsigned long val) { us = micros() - val; return *this; }
};

#endif // EVKB_CM4_ARDUINO_CXX && __cplusplus

// --- DMAMEM: memory a bus master can actually reach --------------------------
//
// USBHost_t36's memory.cpp and ehci_iso.cpp define USBHOST_DMAMEM as DMAMEM
// when __IMXRT1176__ is set, to place the EHCI descriptors (Device_t, Pipe_t,
// Transfer_t, sitd_pool, itd_pool) and payload rings where the controller can
// fetch them. The EHCI controller is its OWN bus master — it does not read
// through the core — and cannot reach TCM at all.
//
// On the CM4 that disqualifies plain .bss twice over. The CM4's DTCM sits at
// 0x20000000, the SAME address the CM7's DTCM occupies in the system map (RM
// Table 3-2 vs Table 3-1), so a CM4-image buffer at a .bss address does not
// name the memory any other master would reach at that address; and that window
// is exactly what qemu2 models as an unreachable hole in the EHCI DMA address
// space (hw/arm/fsl-imxrt1170.c, "usbhost-dtcm-hole"), so a TCM descriptor
// faults loudly in the emulator rather than half-working.
//
// The section name here is the linker's OUTPUT section name directly: the CM4
// script places .bss.dma in OCRAM2 at 0x202C0000 and startup_cm4.S zeroes it.
// OCRAM2 rather than OCRAM1 because OCRAM1 at 0x20240000 is where the CM7's own
// DMAMEM lives (imxrt1176.ld), and the CM4's zeroing loop runs after the CM7
// has booted — see the ★ note in the image's cm4.ld.
//
// The name also differs from the CM7 core, where DMAMEM is ".dmabuffers"
// gathered INTO an output .bss.dma — an indirection a single-purpose CM4 image
// does not need.
#define DMAMEM __attribute__((section(".bss.dma"), used))

// --- print-base constants ----------------------------------------------------
// DEC/HEX/OCT/BIN exist ONLY to satisfy default arguments. USBHost_t36.h writes
//     static void print_(const char *s, int n, uint8_t b = DEC) {}
// and those declarations sit in the #else branch of the USBHOST_PRINT_DEBUG
// guard — so the default argument must still NAME something even though every
// one of these bodies compiles to nothing. Arduino's values.
#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
