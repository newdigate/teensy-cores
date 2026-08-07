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
  setup, DMA, Serial. Peripheral sequences come from a shared-core library
  (e.g. `SPI/lpspi1176`, `Wire/lpi2c1176`) or a probe-backed literal sequence
  in the gate — never from CM7 headers, which assume CM7-side ownership,
  clocking and the CM7 NVIC/vector world.
- CM7-world facilities: RAM vector table, `F_CPU_ACTUAL` runtime clock
  bookkeeping, EventResponder/yield, the allocator, Print/Stream/String.
- Blind copies of `core_pins.h`/`imxrt1176.h` content. Every line here is
  either architecture-defined or an explicit, commented CM4-world decision.
- `#ifdef`-for-CM4 edits to core sources. If a core file cannot compile
  against this shim unmodified, STOP and take it to the coordinator.

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

- **`DEC` / `HEX` / `OCT` / `BIN`** — Phase 7.2, 2026-08-07. `USBHost_t36.h`
  uses `DEC` as a *default argument* (`static void print_(const char *s, int n,
  uint8_t b = DEC) {}`) and those declarations sit in the `#else` branch of the
  `USBHOST_PRINT_DEBUG` guard, so the name must resolve even with debug off
  (`error: 'DEC' was not declared in this scope`). Arduino's values
  (10/16/8/2). They exist *only* to satisfy those default arguments — every one
  of the bodies concerned compiles to nothing. This is not the thin end of a
  `Print`/`Stream` port; that stays on the never-list.
