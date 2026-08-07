# cm4_shim — Arduino-lite for the freestanding CM4 image world

## What this is

`cm4_shim/Arduino.h` is the `Arduino.h` that CM4 **images** (the bare-metal
`-nostdlib` binaries built by `teensy_add_cm4_image`, staged and booted by the
CM7) see when they compile core source files that `#include <Arduino.h>`.
A gate adds it via include-path ordering — the shim directory **before** the
core directory — so `Arduino.h` resolves here while `#include "AudioStream.h"`
still resolves to the real core header:

```cmake
teensy_add_cm4_image(<img> ...
    SOURCES ... ${EVKB_CORES_DIR}/AudioStream.cpp
    INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/cm4
                 ${EVKB_CORES_DIR}/cm4_shim
                 ${EVKB_CORES_DIR})
```

It is deliberately tiny: pure-CPU ARMv7-M primitives that are identical on the
CM4 (PRIMASK, NVIC registers, DWT CYCCNT) plus the repo's `IRQ_SOFTWARE`
convention. First consumer: `AudioStream.cpp` running the audio graph engine
on the CM4 (`examples/dualcore/cm4_audiostream_test`).

`#define EVKB_CM4_WORLD 1` marks the world; image sources may test it, core
sources must not need to (a core file that cannot compile against this shim
unmodified is a design decision for the coordinator, not a shim patch).

## What belongs in it

- Pure-CPU, architecture-defined primitives with **no hardware behind them**:
  interrupt masking, NVIC register access, cycle counter access, plain
  typedefs/macros (`IRQ_NUMBER_t`, `voidFuncPtr`).
- CM4-world semantics for core hooks where the honest implementation differs:
  `attachInterruptVector` is a **no-op** because the CM4 vector table is
  static (in each image's `startup_cm4.S`) — the handler symbol must already
  be in the table at link time.
- Repo conventions the CM7 world also uses (`IRQ_SOFTWARE = 44`, the
  repurposed-CAN1 slot — CAN1 is unused by convention on both cores).
- Declarations a core header needs to *parse* (see the log below), added
  minimally and documented here, one entry per addition.
- A **CM4-local** time base (`millis`/`micros`/`delay`) built on the CM4's own
  DWT at the CM4's own rate. This is the allowed "cycle counter access" with
  wrap bookkeeping on top — not the CM7's clock, whose rate the shim still
  refuses to know. See the log entry for where that line sits.

## What must never go in it

- **Anything with hardware behind it**: peripheral registers, clock/pin/pad
  setup, DMA, Serial. Peripheral *sequences* come from a shared-core library
  (e.g. `SPI/lpspi1176`, `Wire/lpi2c1176`) or a probe-backed literal sequence
  in the gate — never from CM7 headers, which assume CM7-side ownership,
  clocking and the CM7 NVIC/vector world.
- CM7-world facilities: RAM vector table, `F_CPU_ACTUAL` runtime clock
  bookkeeping, EventResponder/yield, the allocator, `String`, and any
  *implementation* of Print/Stream — `HardwareSerial`, buffering, a `printf`.
- Blind copies of `core_pins.h`/`imxrt1176.h` content. Every line here is
  either architecture-defined or an explicit, commented CM4-world decision.

### The line between a facility and a declaration (amended 2026-08-07)

The rule above used to read "Print/Stream/String" flat, and Phase 7.2 hit it
head-on: `USBHost_t36.h` declares `USBSerialBase : public Stream` and a
`JoystickController` member of type `elapsedMicros`, so the header will not
*parse* without those names — even for an image that compiles none of those
drivers.

The distinction that matters is **runtime facility vs. type declaration**:

- **Banned, unchanged.** Anything that brings CM7 machinery: an implementation
  of `write()`, a buffer, `String` and its allocator, `HardwareSerial`.
- **Allowed, opt-in, greppable.** Purely ABSTRACT declarations that exist only
  so a real library's header parses. They cannot be instantiated, they own no
  state, and they must cost the linked image nothing.

`EVKB_CM4_ARDUINO_CXX` is the one such block today: `Print`/`Stream` as five
pure-virtual methods, plus `elapsedMillis`/`elapsedMicros` over the shim's own
clock. **Measured on `cm4_usb_enum_probe`: no vtables, no symbols, image size
unchanged at 10,608 B.** Off by default, so the shim's default surface is
exactly what it was.

**Why not fix it in the library instead**, which is what was done for
`<FS.h>`/`<SdFat.h>` (`USBHOST_NO_MASSSTORAGE`) — the fair comparison, and it
came out the other way. That guard was one contiguous block and removed three
include directories from *every* consumer of the header. Print/Stream would be
two scattered blocks (`USBSerialBase`..`USBSerial_BigBuffer`, and
`USBSerialEmu`), would still not cover `JoystickController`'s `elapsedMicros`,
and would buy back fifty lines of zero-cost scaffolding. Fragmenting a shared
cross-platform header twice more to avoid that is the worse trade.

If a future addition here needs *state* or an *implementation*, that is the
signal the library should be guarded instead. This exception is for
declarations only.
- `#ifdef`-for-CM4 edits to core sources. If a core file cannot compile
  against this shim unmodified, STOP and take it to the coordinator.

### The two OPT-IN exceptions (2026-08-07)

Two of the rules above are now breachable **only by an image that says so**,
via a `-D` in its `teensy_add_cm4_image(... DEFINES ...)` list. Both are off by
default, so the rules stay true for every image that does not name them, and
both flags are greppable so the exceptions stay countable. Today there is
exactly one image using either: `examples/dualcore/cm4_usb_enum_probe`.

| Flag | Breaches | Brings in |
|---|---|---|
| `EVKB_CM4_SOC_REGS` | "no `imxrt1176.h` content", "no peripheral registers" | `#include "imxrt1176.h"` + `IRQ_USB_OTG2`/`IRQ_USB2` |
| `EVKB_CM4_ARDUINO_CXX` | "no Print/Stream/String" | abstract `Print`/`Stream`, `elapsedMillis`/`elapsedMicros` |

**Why they were needed**, and what distinguishes them from the thing the rules
forbid: up to Phase 7.1 every CM4 image drove peripherals from a few
probe-backed literals in its own `main`. Phase 7.2 compiles a whole Arduino
**library** — USBHost_t36 — and a library reaches hardware through the
Teensy-idiom register macros the real `Arduino.h` supplies, and declares its
whole driver family in one header that must parse. Neither is a *behaviour*
import: `imxrt1176.h` is `#define`s plus two no-op cache inlines (a memory map,
and peripheral MMIO answers at the same system addresses from either core, RM
Table 3-1 vs 3-2), and the C++ types are abstract with no vtable emitted, so a
CM4 image still cannot print.

**★ The never-list was not paranoia, and the proof is in the code.** Including
`imxrt1176.h` collides head-on with this shim: the core spells
`NVIC_ENABLE_IRQ`/`NVIC_DISABLE_IRQ`/`NVIC_SET_PENDING`/`NVIC_SET_PRIORITY` as
function-like **macros** and the shim spells them as inline **functions**, so
each macro mangles the function's own definition — dozens of
`expected ')' before 'volatile'` errors pointing at the *core* header, a
thoroughly misleading place to start debugging. `DMAMEM` collides too (the
core's `.dmabuffers` vs this world's `.bss.dma`). Five `#undef`s make it safe;
if a future addition needs more than a handful, that is the signal to stop and
take it to the coordinator rather than keep undefining.

**Guarantee that this is additive:** `cm4_audiostream_test` and `cm4_audio_test`
were force-rebuilt after the change and their `.cm4.bin` sha256s are unchanged
(`d101c608…`, `e161270a…`) — the 2B `cmp` discipline. With the guards in place
that is true by construction, not just by measurement.

## Addition log (beyond the 2026-07-21 plan listing)

Each entry records the compile error that forced the addition.

- `extern volatile uint32_t F_CPU_ACTUAL;` — `AudioStream.h`'s in-class
  inline `processorUsage()`/`processorUsageMax()` expand
  `CYCLE_COUNTER_APPROX_PERCENT`, which names `F_CPU_ACTUAL`; parsing the
  class body fails without a declaration (`AudioStream.h:126:50: error:
  'F_CPU_ACTUAL' was not declared in this scope`, in expansion at
  `AudioStream.h:160`/`:161`, first hit compiling `cm4_audiostream_test`'s
  `main_cm4.cpp`, 2026-07-21). Declaration only, mirroring
  `core_pins.h:2919` — no definition is provided, so an image that actually
  *odr-uses* the CPU-usage percent API fails honestly at link instead of
  silently computing with a fake clock. (`software_isr` itself only stores
  raw `ARM_DWT_CYCCNT` deltas.)

- **`millis()` / `micros()` / `delay()` / `delayMicroseconds()`** — Phase 7.2,
  2026-08-07, forced by USBHost_t36: the compiled subset calls `millis()` 8×,
  `delay()` 4×, `micros()` 2×, `delayMicroseconds()` 2×, and none of them exist
  in this world (`error: 'millis' was not declared in this scope`).

  **On the tension with "F_CPU_ACTUAL runtime clock bookkeeping" above.** The
  line in *What must never go in it* is about the **CM7's** clock: the shim must
  not carry the CM7's bookkeeping and must not pretend to know the CM7's rate.
  That still holds — `F_CPU_ACTUAL` remains declared-but-undefined on purpose,
  so anything reaching for it still fails at link rather than computing with a
  fake number. A **CM4-local** time base is a different thing: it is built on
  the CM4's own DWT — architecture-defined, already on the allowed list as
  "cycle counter access" — at the CM4's own 400 MHz, and it is exactly what a
  freestanding image needs and cannot obtain anywhere else. The 400 MHz lives
  in its own `F_CPU_CM4`, deliberately named so it cannot be mistaken for the
  CM7 constant at a call site.

  Derived from `ARM_DWT_CYCCNT`, never a counted spin loop: `cm4_usb_irq_probe`
  carried one with a "~3 cycles/iter" estimate that was really nearer 7, so a
  nominal 8 s window ran ~18.7 s, collided with the CM7's 20 s receive timeout,
  and truncated a transcript in a way that read as a core hang rather than a bad
  delay. `CYCCNT` is 32-bit and wraps every 10.74 s at 400 MHz, so each call
  *folds* the elapsed-cycle delta into software µs/ms counters instead of
  dividing the raw counter; the fold runs under PRIMASK save/restore because
  USBHost_t36 calls `millis()` from its ISR as well as main context. State is
  `__attribute__((weak))` at file scope, not function-local statics, so all
  translation units share one epoch.

  **Two limits a future reader will hit:**
  1. **The image must enable DWT** (`DEMCR.TRCENA`, then `DWT_CTRL.CYCCNTENA`)
     before *any* clock call. Without it `CYCCNT` stays 0, every clock stands
     still, and USBHost_t36's timeouts silently never fire — nothing errors.
     Enable it first thing in init, as the gates' `dwt_start()` does.
  2. **Something must call `millis()`/`micros()` at least every ~10.7 s** or
     whole wraps are lost: only one wrap can be inferred per call. `delay()`
     polls `millis()`, so long delays are safe; an otherwise idle image must
     poll.

- **`DMAMEM`** — Phase 7.2, 2026-08-07. `USBHost_t36/memory.cpp` and
  `ehci_iso.cpp` define `USBHOST_DMAMEM` as `DMAMEM` under `__IMXRT1176__`
  (`error: expected ';' before 'Device_t'` without it). Defined here as
  `__attribute__((section(".bss.dma"), used))`. This is *not* a hardware
  facility — it is a placement attribute — but the reason it must exist is:
  the EHCI controller is its own bus master and cannot reach either TCM, and on
  the CM4 plain `.bss` is doubly disqualified because the CM4's DTCM sits at
  `0x20000000`, the same address the CM7's DTCM occupies in the system map, and
  that window is what qemu2 models as an unreachable hole in the EHCI DMA
  address space (`usbhost-dtcm-hole`). Unlike the CM7 core (`.dmabuffers`
  gathered into an output `.bss.dma`), the name here *is* the output section
  name; each CM4 image's linker script places it. `cm4_usb_irq_probe`'s
  `cm4.ld` puts it in **OCRAM2 at 0x202C0000** — deliberately not OCRAM1, which
  is where the CM7's own DMAMEM lives; see the ★ note in that file.

- **`EVKB_CM4_SOC_REGS` / `EVKB_CM4_ARDUINO_CXX`** — Phase 7.2c, 2026-08-07,
  forced by compiling the USBHost_t36 *transport core* itself (not just calling
  into it). See "The two OPT-IN exceptions" above for the policy argument; the
  compile errors that forced each were:

  - `ehci.cpp:237: error: 'CCM_LPCG115_DIRECT' was not declared in this scope`,
    plus `USBPHY2_CTRL_CLR`, `USBPHY_CTRL_SFTRST`, `USBPHY2_PLL_SIC*`,
    `USBPHY_PLL_SIC_PLL_*`, `USB2_*` via `utility/imxrt_usbhs.h`, and
    `IRQ_USBHS`→`IRQ_USB2` at `ehci.cpp:324-325` — about 70 names in all.
  - `USBHost_t36.h:1595: error: expected class-name before '{' token`
    (`USBSerialBase : public USBDriver, public Stream`), `:1637: error: 'Print'
    has not been declared` (`using Print::write;`), the same pair again at
    `:2014`/`:2034` for `USBSerialEmu`, and `:1189: error: 'elapsedMicros' does
    not name a type`.

  ★ **A trap worth not rediscovering:** the core's own `elapsedMillis.h` could
  NOT simply be `#include`d to satisfy the last one. It opens with a *quoted*
  `#include "Arduino.h"`, and a quoted include searches the **including file's
  own directory first**, ahead of every `-I` — so from `cores/imxrt1176/` it
  resolves to the CM7 `Arduino.h`, drags in `core_pins.h`, and collides
  (`using typedef-name 'IRQ_NUMBER_t' after 'enum'`, `conflicting declaration
  of 'void __enable_irq()' with 'C' linkage`). The INCLUDE_DIRS ordering that
  makes this shim win for `<Arduino.h>` cannot help; it is out-ranked by the
  includer's directory. Hence the local class definitions.

- **`DEC` / `HEX` / `OCT` / `BIN`** — Phase 7.2, 2026-08-07. `USBHost_t36.h`
  uses `DEC` as a *default argument* (`static void print_(const char *s, int n,
  uint8_t b = DEC) {}`) and those declarations sit in the `#else` branch of the
  `USBHOST_PRINT_DEBUG` guard, so the name must resolve even with debug off
  (`error: 'DEC' was not declared in this scope`). Arduino's values
  (10/16/8/2). They exist *only* to satisfy those default arguments — every one
  of the bodies concerned compiles to nothing. This is not the thin end of a
  `Print`/`Stream` port; that stays on the never-list.
