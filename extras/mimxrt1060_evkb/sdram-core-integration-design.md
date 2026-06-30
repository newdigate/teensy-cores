# MIMXRT1060-EVKB — SDRAM core integration (design)

**Date:** 2026-06-30
**Status:** approved, pending implementation plan

## Goal

Bring up the EVKB's on-board **32 MB SEMC SDRAM** in the `teensy4` core so it is
usable from sketches through the standard Teensy mechanisms:

- the `EXTMEM` attribute places static globals in SDRAM, and
- `extmem_malloc()` / `extmem_free()` / `extmem_calloc()` / `extmem_realloc()`
  allocate from an SDRAM heap,

exactly like Teensy 4.1's FlexSPI2 PSRAM — **plus** the `EXTMEM` `.bss` is
zero-initialized at boot (Teensy 4.1 leaves uninitialized `EXTMEM` globals as
garbage; this avoids that footgun).

## Background (all hardware-verified this session)

- The EVKB has **32 MB SDRAM (ISSI IS42S16160J-class) on the SEMC controller at
  `0x80000000`**: 16-bit port, CS0, 9-bit column, BL8, CAS-3.
- A standalone Arduino bring-up (`scratchpad/semctest/semctest.ino`) passes a full
  32 MB memtest (index/0x55/0xAA/0x00/0xFF) with **0 errors**.
- **Key gotcha — `DQSMD` (MCR bit2) must be 0** (internal read-strobe loopback).
  The NXP SDK uses `DQSMD=1` (DQS-pad loopback, "for accuracy"), but that path
  does not work in the bare Teensy environment: the controller configures
  perfectly and every read still returns `0x00000000`. `DQSMD=0` fixes it.
- **Clock:** SEMC = PLL2 PFD2 (396 MHz) ÷ 3 = **132 MHz**. **PFD2 must NOT be
  reprogrammed** — it also clocks the boot flash; changing it mid-XIP crashes the
  board. The `SEMC_PODF` change requires the `CCM_CDHIPR_SEMC_PODF_BUSY` handshake.
- **MCR bit layout:** bit0 = SWRST, bit1 = MDIS (module disable), bit2 = DQSMD.
- The core's `configure_external_ram()` is currently compiled for the EVKB and
  muxes **EMC_22–29 to FlexSPI2** — but on the EVKB those are SDRAM control pins
  (BA1, ADDR10, CAS, RAS, CLK, CKE, WE, CS0). This integration removes that path
  for the EVKB, so the conflict disappears at the source.
- **MPU:** Teensy already maps `0x80000000` (region: `MEM_CACHE_WBWA | READWRITE |
  NOEXEC | SIZE_1G`, "SEMC: SDRAM, NAND, SRAM"). **No MPU change needed.** Use
  `arm_dcache_*` around bulk SDRAM access.
- **Boot order (verified in `startup.c`):** `CCM_ANALOG_PFD_528` set at L116 →
  `configure_cache()` (enables MPU) at L162 → `set_arm_clock()` at L166 →
  `configure_external_ram()` at L187. So **PFD2 (396 MHz) and the `0x80000000`
  MPU region are both live** when SEMC init runs. `set_arm_clock()` changes
  PLL_ARM only, not PFD2, so CPU-speed changes don't disturb the 132 MHz SEMC clock.

### Golden SEMC register values @ 132 MHz

Dumped from NXP's `semc_sdram` SDK example reconfigured to 132 MHz (where it
passes), so the timing fields match this exact clock:

| Reg | Value | Notes |
|---|---|---|
| `MCR` | `0x1FFF0000` | enable (MDIS=0), **DQSMD=0**, BTO/CTO |
| `BMCR0` | `0x00104085` | |
| `BMCR1` | `0x40246085` | |
| `IOCR` | `0x00000000` | |
| `BR0` | `0x8000001B` | base 0x80000000, 32 MB (MS=13), valid |
| `SDRAMCR0` | `0x00000F31` | 16-bit, BL8, 9-col, CAS3 |
| `SDRAMCR1` | `0x00440711` | timings @132 MHz |
| `SDRAMCR2` | `0x00060607` | |
| `SDRAMCR3` | `0x06050A00` → `…A01` | REN=0 during init, then enable refresh |
| `IPCR1` | `0x00000002` | |

IP init sequence (`IPCMD = cmd | (0xA55A << 16)`, poll `INTR` bit0): precharge-all
(`0xF`), auto-refresh ×8 (`0xC`), mode-set (`0xA`, data `0x33` = BL8|CAS3).

## Approach

**A — isolate the register-level init in a new `teensy4/semc.c`; thin EVKB branch
in `configure_external_ram()` reuses the existing pool/zero logic.** (Chosen over
inlining ~60 lines of registers into `startup.c`, or duplicating the whole
function for the EVKB.)

## Touch points

### 1. `teensy4/semc.c` + `teensy4/semc.h` (new)

`FLASHMEM void extram_semc_init(void)` performs the proven bring-up:

1. Pin mux: `EMC_00..EMC_41` → ALT0 (SEMC), pad ctl `0x110F9`
   (HYS | keeper | 200 MHz | DSE7 | fast). Loop over the contiguous
   `IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_00..41` / `SW_PAD_CTL_...` registers.
2. Clock: route SEMC = PLL2 PFD2 (alt, `SEMC_ALT_CLK_SEL=0`) ÷3
   (`SEMC_CLK_SEL=1`, `SEMC_PODF=2`) with the `CCM_CDHIPR_SEMC_PODF_BUSY`
   handshake; enable `CCM_CCGR3` SEMC gate. **PFD2 untouched.**
3. SWRST (MCR bit0), poll clear; configure with MDIS=1 then enable (MDIS=0),
   `DQSMD=0`; write `BR0`, `SDRAMCR0-3` (REN off), `IPCR1`, etc.
4. JEDEC power-up wait (see Risks), then precharge / 8× refresh / mode-set;
   enable refresh (`SDRAMCR3 |= REN`).

Constraints: `FLASHMEM`, register writes only — **no `Serial`, no heap, no
`delay()`/`millis()`** (see Risks). Uses Teensy `imxrt.h` SEMC macros
(`IMXRT_SEMC` @ `0x402F0000`).

### 2. `teensy4/startup.c` — `configure_external_ram()`

Split the EVKB out of the shared body:

- `#if defined(ARDUINO_TEENSY41)` → existing FlexSPI2 PSRAM body, unchanged.
- `#elif defined(ARDUINO_MIMXRT1060_EVKB)` →
  1. `extram_semc_init();`
  2. zero the static `EXTMEM` globals: `memset(&_extram_start, 0,
     (uint32_t)&_extram_end - (uint32_t)&_extram_start);`
  3. `external_psram_size = 32;`
  4. `sm_set_pool(&extmem_smalloc_pool, &_extram_end, 32 * 0x100000 -
     ((uint32_t)&_extram_end - (uint32_t)&_extram_start), 1, NULL);`

The EVKB branch does **not** touch FlexSPI2, so EMC_22–29 are never muxed away
from SEMC.

### 3. `teensy4/extmem.c` — `IS_EXTMEM`

The EVKB SDRAM is at `0x80000000` (`addr >> 28 == 8`), not Teensy 4.1's
`0x70000000` (`== 7`). Make `IS_EXTMEM` board-specific:

```c
#if defined(ARDUINO_TEENSY41)
#define IS_EXTMEM(addr) (((uint32_t)(addr) >> 28) == 7)
#elif defined(ARDUINO_MIMXRT1060_EVKB)
#define IS_EXTMEM(addr) (((uint32_t)(addr) >> 28) == 8)
#endif
```

`HAS_EXTRAM` stays defined for both. `extmem_free`/`extmem_realloc` route
correctly via the updated macro.

### 4. `teensy4/imxrt1060_evkb.ld`

Change `ERAM` origin from the placeholder `0x70000000` to the real SDRAM:

```
ERAM (rw):  ORIGIN = 0x80000000, LENGTH = 32768K
```

Length and the `.bss.extram` placement / `_extram_start` / `_extram_end` symbols
are unchanged — they now land in real SDRAM. Update the file's header comment
(it currently says bring-up is deferred).

## Data flow

`EXTMEM int buf[N];` → linker section `.externalram` → `.bss.extram` at
`0x80000000` (zeroed at boot). `extmem_malloc(n)` → `extmem_smalloc_pool` carved
from `_extram_end` to end-of-SDRAM. Both resolve to SEMC SDRAM, transparently to
the sketch.

## Scope

**In:** `EXTMEM` static globals in SDRAM (zero-initialized); `extmem_malloc`
family from an SDRAM heap; SEMC bring-up at boot for the EVKB.

**Out (matches upstream Teensy's unfinished TODO):** copy-from-flash
initialization of `EXTMEM` globals that have *explicit* initializers
(`EXTMEM int x = 5;`). The linker emits only `.bss.extram`; such a global would be
zeroed, not value-initialized. Documented as a known limitation, same as Teensy 4.1.

## Testing

- **Hardware (primary):** (a) the proven 32 MB memtest still passes after moving
  the init into the core; (b) a new sketch exercising an `EXTMEM` global *and*
  `extmem_malloc()` — write, read back, verify — confirming end-to-end parity.
  Flash via the DAPLink `cp` workflow; read `Serial6`.
- **QEMU (smoke):** QEMU backs `0x80000000` with plain RAM and stubs the SEMC
  controller, so `EXTMEM`/`malloc` work there trivially. This confirms **no boot
  regression** but does **not** validate the SEMC init (hardware-only, like USB
  host). Run the existing EVKB QEMU sketch to ensure boot still reaches `loop()`.
- **Build:** `extras/mimxrt1060_evkb/build_check.sh` compiles clean for the EVKB;
  the standard `Blink`/examples still build.

## Risks / open items

- **Early-boot timing source.** `extram_semc_init()` runs at `startup.c:187`. The
  JEDEC power-up wait and any poll loops must **not** rely on `delay()`/`millis()`
  if the systick/`millis` ISR isn't running yet. Resolve in the plan: use the DWT
  cycle counter (`ARM_DWT_CYCCNT`, if `TRCENA`/`CYCCNTENA` are enabled by then) or
  a conservative calibrated busy-loop (≥200 µs at 600 MHz). Bounded guards already
  exist on the SWRST and IP-command polls.
- **`FLASHMEM` callee.** `extram_semc_init()` and `semc.c` must be `FLASHMEM` and
  self-contained (no ITCM-resident helpers assumed before init).
- **No behavior change for Teensy 4.0/4.1.** All edits are under
  `ARDUINO_MIMXRT1060_EVKB` guards or the new file; the Teensy paths are untouched.
