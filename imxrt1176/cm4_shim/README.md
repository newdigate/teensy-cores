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
