# EVKB SDRAM Core Integration — Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Bring up the EVKB's 32 MB SEMC SDRAM in the `teensy4` core so `EXTMEM` globals (zero-initialized at boot) and `extmem_malloc()` resolve to the SDRAM, like Teensy 4.1 PSRAM.

**Architecture:** A new `teensy4/semc.c` performs the SEMC register-level bring-up (clock = PLL2 PFD2 ÷3 = 132 MHz, DQSMD=0). The EVKB branch of `configure_external_ram()` (startup.c) calls it, zeroes the `EXTMEM` `.bss`, sets `external_psram_size = 32`, and builds the `extmem_smalloc_pool`. `extmem.c` and the EVKB linker script are adjusted from the FlexSPI2 PSRAM address (0x70000000) to the SDRAM (0x80000000).

**Tech Stack:** C, i.MX RT1062 SEMC + CCM registers (Teensy `imxrt.h` macros), GNU linker script, `arduino-cli` + the DAPLink `cp` flash workflow, `sercap2.py` serial capture.

**Design reference:** `extras/mimxrt1060_evkb/sdram-core-integration-design.md`

---

## Dev workflow (every build/flash/verify step uses this)

`arduino-cli` compiles the **installed platform**, not this repo. So changed core files must be synced to the platform before each build. Define these once per shell:

```bash
REPO=/Users/nicholasnewdigate/Development/rt1060/evkb/cores/.claude/worktrees/lucid-bohr-145339
PLAT=/Users/nicholasnewdigate/Library/Arduino15/packages/teensy/hardware/avr/1.59.0/cores/teensy4
SCRATCH=/private/tmp/claude-501/-Users-nicholasnewdigate-Development-rt1060-evkb-cores--claude-worktrees-lucid-bohr-145339/d2e80e15-e3f5-4bc2-9b15-afcf9978acbc/scratchpad
sync() { cp "$REPO"/teensy4/semc.c "$REPO"/teensy4/semc.h "$REPO"/teensy4/startup.c \
            "$REPO"/teensy4/extmem.c "$REPO"/teensy4/imxrt1060_evkb.ld "$PLAT"/ 2>/dev/null; }
```

- **Build:** `arduino-cli compile -b teensy:avr:mimxrt1060evkb --build-path <bp> <sketchdir>`
- **Flash:** `cp <bp>/<sketch>.ino.hex /Volumes/RT1060-EVK/` (DAPLink programs + auto-resets)
- **Capture (do it AFTER the flash settles, with a fresh reader):** `python3 "$SCRATCH"/sercap2.py /dev/cu.usbmodem145402 <secs>` — the first post-flash capture catches the reset transient; re-capture once.

---

## File structure

- **Create** `teensy4/semc.h` — prototype for `extram_semc_init()`.
- **Create** `teensy4/semc.c` — SEMC SDRAM bring-up (FLASHMEM, register-only).
- **Modify** `teensy4/startup.c` — guard FlexSPI2 helpers for Teensy 4.1 only; branch `configure_external_ram()` for the EVKB; `#include "semc.h"`.
- **Modify** `teensy4/extmem.c` — board-specific `IS_EXTMEM`.
- **Modify** `teensy4/imxrt1060_evkb.ld` — `ERAM` origin 0x70000000 → 0x80000000.
- **Create** `extras/mimxrt1060_evkb/sdram_parity_test/sdram_parity_test.ino` — hardware integration test.

---

## Task 1: Linker — move ERAM to the real SDRAM

**Files:**
- Modify: `teensy4/imxrt1060_evkb.ld` (MEMORY block + header comment)

- [ ] **Step 1: Edit the ERAM origin**

In `teensy4/imxrt1060_evkb.ld`, change the `ERAM` line in the `MEMORY` block from:

```
	ERAM (rw):  ORIGIN = 0x70000000, LENGTH = 32768K
```

to:

```
	ERAM (rw):  ORIGIN = 0x80000000, LENGTH = 32768K
```

- [ ] **Step 2: Update the header comment**

Replace the file's top comment (lines ~3–5, the one stating "SDRAM bring-up is deferred. The ERAM ... is unused") with:

```
 * 32 MB SDRAM on SEMC @ 0x80000000. The core brings it up in
 * configure_external_ram() (startup.c -> extram_semc_init() in semc.c), so the
 * ERAM region and .bss.extram / _extram_start / _extram_end land in real SDRAM.
```

- [ ] **Step 3: Verify it builds and places EXTMEM at 0x80000000**

```bash
mkdir -p "$SCRATCH"/ld_probe/ld_probe
cat > "$SCRATCH"/ld_probe/ld_probe/ld_probe.ino <<'EOF'
EXTMEM unsigned int probe[256];
void setup(){ Serial6.begin(115200); Serial6.println(probe[0]); }
void loop(){}
EOF
sync
arduino-cli compile -b teensy:avr:mimxrt1060evkb --build-path "$SCRATCH"/ld_probe_build "$SCRATCH"/ld_probe/ld_probe 2>&1 | tail -3
arm-none-eabi-nm "$SCRATCH"/ld_probe_build/ld_probe.ino.elf 2>/dev/null | grep -i probe || \
  /Users/nicholasnewdigate/Library/Arduino15/packages/teensy/tools/teensy-compile/11.3.1/arm/bin/arm-none-eabi-nm "$SCRATCH"/ld_probe_build/ld_probe.ino.elf | grep -i ' probe'
```

Expected: compiles clean, and `probe` symbol address starts with `8` (e.g. `80000000 B probe`).

- [ ] **Step 4: Commit**

```bash
cd "$REPO" && git add teensy4/imxrt1060_evkb.ld
git commit -m "EVKB: point ERAM linker region at the real SEMC SDRAM (0x80000000)"
```

---

## Task 2: New `semc.c` / `semc.h` — SEMC SDRAM bring-up

**Files:**
- Create: `teensy4/semc.h`
- Create: `teensy4/semc.c`

- [ ] **Step 1: Create `teensy4/semc.h`**

```c
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
```

- [ ] **Step 2: Create `teensy4/semc.c`**

```c
// teensy4/semc.c - MIMXRT1060-EVKB SEMC SDRAM bring-up.
// 32 MB SDRAM (ISSI IS42S16160J-class) @ 0x80000000: 16-bit, CS0, 9-col, BL8, CAS3.
// Clocked from PLL2 PFD2 (396 MHz, left untouched - it also clocks the boot flash) / 3
// = 132 MHz. Register values dumped from NXP's semc_sdram example reconfigured to 132 MHz.
// MCR bit layout: bit0=SWRST, bit1=MDIS, bit2=DQSMD. DQSMD MUST be 0 (internal read-strobe
// loopback); the SDK's DQSMD=1 (DQS-pad loopback) returns all-zero reads on this board.
#if defined(ARDUINO_MIMXRT1060_EVKB)
#include "imxrt.h"
#include "semc.h"

// One SEMC IP command (cmd low nibble: 0xF=precharge-all, 0xC=auto-refresh, 0xA=mode-set).
// Bounded poll so a non-responding controller (e.g. a QEMU stub) can't hang boot.
static void semc_ipcmd(uint32_t addr, uint32_t cmd, uint32_t data)
{
	SEMC_INTR = 0x1;                       // clear IPCMDDONE (write-1-clear)
	SEMC_IPCR0 = addr;
	uint32_t m = cmd & 0xF;
	if (m == 0xA || m == 0x9) SEMC_IPTXDAT = data;   // mode-set / write carry data
	SEMC_IPCMD = cmd | (0xA55Au << 16);    // SEMC_IPCMD magic key
	uint32_t guard = 2000000;
	while (!(SEMC_INTR & 0x1) && --guard) { }
}

FLASHMEM void extram_semc_init(void)
{
	// Pins: EMC_00..EMC_41 -> ALT0 (SEMC), strong/fast pads (HYS|keeper|200MHz|DSE7|fast).
	volatile uint32_t *mux = &IOMUXC_SW_MUX_CTL_PAD_GPIO_EMC_00;
	volatile uint32_t *pad = &IOMUXC_SW_PAD_CTL_PAD_GPIO_EMC_00;
	for (int i = 0; i < 42; i++) { mux[i] = 0; pad[i] = 0x110F9; }

	// Clock: SEMC = PLL2 PFD2 / 3 = 132 MHz. PFD2 untouched. SEMC_PODF needs the handshake.
	CCM_CBCDR = (CCM_CBCDR & ~CCM_CBCDR_SEMC_ALT_CLK_SEL) | CCM_CBCDR_SEMC_CLK_SEL; // alt=PFD2
	CCM_CBCDR = (CCM_CBCDR & ~CCM_CBCDR_SEMC_PODF(7)) | CCM_CBCDR_SEMC_PODF(2);     // /3
	uint32_t hg = 2000000;
	while ((CCM_CDHIPR & CCM_CDHIPR_SEMC_PODF_BUSY) && --hg) { }
	CCM_CCGR3 |= CCM_CCGR3_SEMC(3);                                                 // gate on

	// Soft reset (SWRST=bit0), then configure disabled (MDIS=1) and enable (MDIS=0); DQSMD=0.
	SEMC_MCR = 0x1u;
	uint32_t guard = 2000000;
	while ((SEMC_MCR & 0x1u) && --guard) { }
	SEMC_MCR   = 0x1FFF0002;   // MDIS=1, DQSMD=0, BTO/CTO
	SEMC_BMCR0 = 0x00104085;
	SEMC_BMCR1 = 0x40246085;
	SEMC_MCR   = 0x1FFF0000;   // enable (MDIS=0), DQSMD=0
	SEMC_IOCR     = 0x00000000;
	SEMC_INTEN    = 0x00000000;
	SEMC_BR0      = 0x8000001B; // base 0x80000000, 32 MB, valid
	SEMC_SDRAMCR0 = 0x00000F31; // 16-bit, BL8, 9-col, CAS3
	SEMC_SDRAMCR1 = 0x00440711; // timings @132 MHz
	SEMC_SDRAMCR2 = 0x00060607;
	SEMC_SDRAMCR3 = 0x06050A00; // refresh disabled (REN=0) during init
	SEMC_IPCR1    = 0x00000002;

	// JEDEC power-up: >100us stable clock before init commands. Busy-loop because delay()/
	// the DWT cycle counter are not running yet this early in boot; this is >=100us at any
	// core clock (200k NOPs ~= 0.67 ms at 600 MHz, more when slower).
	for (volatile uint32_t w = 0; w < 200000; w++) { __asm__ volatile("nop"); }

	semc_ipcmd(0x80000000UL, 0xF, 0);                                  // precharge all
	for (int r = 0; r < 8; r++) semc_ipcmd(0x80000000UL, 0xC, 0);      // auto-refresh x8
	semc_ipcmd(0x80000000UL, 0xA, 0x33);                               // mode-set: BL8 | CAS3

	SEMC_SDRAMCR3 = 0x06050A01; // enable auto-refresh (REN=1)
}
#endif // ARDUINO_MIMXRT1060_EVKB
```

- [ ] **Step 3: Verify it compiles as part of the core**

(It isn't called yet; this just confirms the file compiles in the core build.)

```bash
sync
arduino-cli compile -b teensy:avr:mimxrt1060evkb --build-path "$SCRATCH"/ld_probe_build "$SCRATCH"/ld_probe/ld_probe 2>&1 | grep -iE 'error|semc' ; echo "exit ${PIPESTATUS[0]}"
```

Expected: no errors (semc.c compiles; `extram_semc_init` is unused-but-defined, which is fine — it's non-static and FLASHMEM).

- [ ] **Step 4: Commit**

```bash
cd "$REPO" && git add teensy4/semc.c teensy4/semc.h
git commit -m "EVKB: add semc.c - SEMC SDRAM bring-up (132 MHz, DQSMD=0)"
```

---

## Task 3: Wire it into `configure_external_ram()`

**Files:**
- Modify: `teensy4/startup.c` (the `#if defined(ARDUINO_TEENSY41) || defined(ARDUINO_MIMXRT1060_EVKB)` block at ~line 353; `configure_external_ram()` at ~line 423)

- [ ] **Step 1: Add the `semc.h` include**

Near the top of `teensy4/startup.c`, with the other `#include` lines, add:

```c
#include "semc.h"
```

- [ ] **Step 2: Restrict the FlexSPI2 PSRAM helpers to Teensy 4.1**

The LUT macros + `flexspi2_command()` + `flexspi2_psram_size()` are FlexSPI2-only. Immediately **after** the line `#if defined(ARDUINO_TEENSY41) || defined(ARDUINO_MIMXRT1060_EVKB)` (~line 353), add:

```c
#if defined(ARDUINO_TEENSY41)
```

and immediately **before** `FLASHMEM void configure_external_ram()` (~line 423), add the matching close:

```c
#endif // ARDUINO_TEENSY41 (FlexSPI2 PSRAM helpers)
```

This leaves `configure_external_ram()` (and the outer `#if ... || ARDUINO_MIMXRT1060_EVKB`) compiled for both boards, but the FlexSPI2 helper functions only for Teensy 4.1.

- [ ] **Step 3: Branch the body of `configure_external_ram()`**

Replace the entire body of `FLASHMEM void configure_external_ram()` (everything between its opening `{` and closing `}`) with:

```c
{
#if defined(ARDUINO_MIMXRT1060_EVKB)
	// EVKB: 32 MB SDRAM on SEMC @ 0x80000000 (NOT FlexSPI2 PSRAM).
	extram_semc_init();
	uint32_t extram_bytes = (uint32_t)&_extram_end - (uint32_t)&_extram_start;
	if (extram_bytes) memset(&_extram_start, 0, extram_bytes); // zero EXTMEM .bss globals
	external_psram_size = 32;
	sm_set_pool(&extmem_smalloc_pool, &_extram_end,
		32 * 0x100000 - extram_bytes, 1, NULL);
#else
	// ... existing Teensy 4.1 FlexSPI2 PSRAM body, verbatim (pin mux, clock, FlexSPI2
	//     setup, LUT, flexspi2_psram_size probe, sm_set_pool / external_psram_size) ...
#endif
}
```

When implementing: keep the original Teensy 4.1 body exactly as it was, moved unchanged into the `#else` branch. Do not alter it.

- [ ] **Step 4: Verify compile + that EXTMEM is now wired**

```bash
sync
arduino-cli compile -b teensy:avr:mimxrt1060evkb --build-path "$SCRATCH"/ld_probe_build "$SCRATCH"/ld_probe/ld_probe 2>&1 | grep -iE 'error' ; echo "exit ${PIPESTATUS[0]}"
```

Expected: no errors. (`memset`, `sm_set_pool`, `external_psram_size`, `extmem_smalloc_pool`, `_extram_start/_end` are all already declared/used in this file.)

- [ ] **Step 5: Commit**

```bash
cd "$REPO" && git add teensy4/startup.c
git commit -m "EVKB: configure_external_ram() brings up SEMC SDRAM + EXTMEM pool"
```

---

## Task 4: Point `extmem.c` at the SDRAM address

**Files:**
- Modify: `teensy4/extmem.c:38-42`

- [ ] **Step 1: Make `IS_EXTMEM` board-specific**

Replace:

```c
#if defined(ARDUINO_TEENSY41) || defined(ARDUINO_MIMXRT1060_EVKB)
// Teensy 4.1 external RAM address range is 0x70000000 to 0x7FFFFFFF
#define HAS_EXTRAM
#define IS_EXTMEM(addr) (((uint32_t)(addr) >> 28) == 7)
#endif
```

with:

```c
#if defined(ARDUINO_TEENSY41) || defined(ARDUINO_MIMXRT1060_EVKB)
#define HAS_EXTRAM
#if defined(ARDUINO_MIMXRT1060_EVKB)
// EVKB external RAM is 32 MB SEMC SDRAM at 0x80000000..0x81FFFFFF
#define IS_EXTMEM(addr) (((uint32_t)(addr) >> 28) == 8)
#else
// Teensy 4.1 external RAM (FlexSPI2 PSRAM) is 0x70000000..0x7FFFFFFF
#define IS_EXTMEM(addr) (((uint32_t)(addr) >> 28) == 7)
#endif
#endif
```

- [ ] **Step 2: Verify compile**

```bash
sync
arduino-cli compile -b teensy:avr:mimxrt1060evkb --build-path "$SCRATCH"/ld_probe_build "$SCRATCH"/ld_probe/ld_probe 2>&1 | grep -iE 'error' ; echo "exit ${PIPESTATUS[0]}"
```

Expected: no errors.

- [ ] **Step 3: Commit**

```bash
cd "$REPO" && git add teensy4/extmem.c
git commit -m "EVKB: extmem_free/realloc recognise SDRAM at 0x80000000"
```

---

## Task 5: Hardware integration test (the real test)

This sketch does **not** init SEMC — it relies on the core. It verifies: the `EXTMEM` global lands in SDRAM and reads 0 at boot (zero-init), `EXTMEM` read/write works, and `extmem_malloc()` returns SDRAM that reads/writes correctly. Before Tasks 1–4 this test fails (EXTMEM at 0x70000000 → garbage/fault); after, it passes.

**Files:**
- Create: `extras/mimxrt1060_evkb/sdram_parity_test/sdram_parity_test.ino`

- [ ] **Step 1: Write the test sketch**

```cpp
// EVKB SDRAM core-integration parity test. The CORE brings up the SEMC SDRAM;
// this sketch does NOT touch SEMC. Output on Serial6 (115200).
#include <Arduino.h>

extern "C" {
  extern uint8_t external_psram_size;
  void *extmem_malloc(size_t size);
}

EXTMEM uint32_t exglobal[4096];   // static EXTMEM global -> .bss.extram @ 0x80000000

void setup() {
  Serial6.begin(115200);
  uint32_t t = millis();
  while (!Serial6 && (millis() - t) < 2000) ;
  Serial6.println("\n\n=== EVKB SDRAM core-integration parity test ===");
  Serial6.printf("external_psram_size = %u MB\n", external_psram_size);
  Serial6.printf("exglobal @ 0x%08X\n", (uint32_t)exglobal);

  // 1) EXTMEM global must be in SDRAM (0x8xxxxxxx) and zeroed at boot.
  bool inSdram = ((uint32_t)exglobal >> 28) == 8;
  arm_dcache_delete(exglobal, sizeof(exglobal));   // read SDRAM, not stale cache
  uint32_t nonzero = 0;
  for (uint32_t i = 0; i < 4096; i++) if (exglobal[i] != 0) nonzero++;
  Serial6.printf("EXTMEM global: inSdram=%d  nonzero-at-boot=%lu (want 0)\n",
                 inSdram, (unsigned long)nonzero);

  // 2) Write/read the EXTMEM global.
  for (uint32_t i = 0; i < 4096; i++) exglobal[i] = i * 2654435761u;
  arm_dcache_flush_delete(exglobal, sizeof(exglobal));
  uint32_t gerr = 0;
  for (uint32_t i = 0; i < 4096; i++) if (exglobal[i] != i * 2654435761u) gerr++;
  Serial6.printf("EXTMEM global write/read errors=%lu\n", (unsigned long)gerr);

  // 3) extmem_malloc a 4 MB chunk and test it.
  const uint32_t N = 1024 * 1024;            // 1M uint32 = 4 MB
  uint32_t *heap = (uint32_t *)extmem_malloc(N * sizeof(uint32_t));
  uint32_t herr = 0; bool heapInSdram = false;
  if (heap) {
    heapInSdram = ((uint32_t)heap >> 28) == 8;
    for (uint32_t i = 0; i < N; i++) heap[i] = ~i;
    arm_dcache_flush_delete(heap, N * sizeof(uint32_t));
    for (uint32_t i = 0; i < N; i++) if (heap[i] != ~i) herr++;
  }
  Serial6.printf("extmem_malloc(4MB)=%p inSdram=%d errors=%lu\n",
                 (void*)heap, heapInSdram, (unsigned long)herr);

  bool pass = inSdram && (nonzero == 0) && (gerr == 0) &&
              heap && heapInSdram && (herr == 0) && (external_psram_size == 32);
  Serial6.printf("RESULT: %s\n", pass ? "PARITY PASS" : "PARITY FAIL");
  Serial6.flush();
}

void loop() {}
```

- [ ] **Step 2: Build, flash, and capture**

```bash
sync
arduino-cli compile -b teensy:avr:mimxrt1060evkb --build-path "$SCRATCH"/parity_build \
  "$REPO"/extras/mimxrt1060_evkb/sdram_parity_test 2>&1 | grep -iE 'error|Memory Usage'
cp "$SCRATCH"/parity_build/sdram_parity_test.ino.hex /Volumes/RT1060-EVK/
python3 "$SCRATCH"/sercap2.py /dev/cu.usbmodem145402 8   # re-run once if first catches the reset
```

Expected output:

```
external_psram_size = 32 MB
exglobal @ 0x800000xx
EXTMEM global: inSdram=1  nonzero-at-boot=0 (want 0)
EXTMEM global write/read errors=0
extmem_malloc(4MB)=0x800... inSdram=1 errors=0
RESULT: PARITY PASS
```

If `RESULT: PARITY FAIL` or reads are zero/garbage: re-check Task 2 (`DQSMD` must be 0) and Task 1 (ERAM origin). Use the standalone `scratchpad/semctest` register dump approach to compare SEMC registers if needed.

- [ ] **Step 3: Commit**

```bash
cd "$REPO" && git add extras/mimxrt1060_evkb/sdram_parity_test/sdram_parity_test.ino
git commit -m "EVKB: add SDRAM EXTMEM/extmem_malloc parity test sketch"
```

---

## Task 6: QEMU smoke test, build check, docs, finalize

**Files:**
- Modify: `extras/mimxrt1060_evkb/README.md` (add an SDRAM section)
- Modify: `extras/mimxrt1060_evkb/LIBRARY_SUPPORT.md` (note core SDRAM/EXTMEM support if a relevant row exists)

- [ ] **Step 1: QEMU smoke test (no boot regression)**

QEMU backs 0x80000000 with plain RAM and stubs the SEMC controller, so `EXTMEM`/`malloc` work trivially and the bounded SEMC polls just spin to their guard then continue. Confirm boot still reaches `setup()` and prints `PARITY PASS` (RAM-backed):

```bash
qemu-system-arm -M mimxrt1060-evk -kernel "$SCRATCH"/parity_build/sdram_parity_test.ino.elf \
  -nographic -monitor none -serial null -serial stdio 2>&1 | head -20 &
sleep 6; kill %1 2>/dev/null
```

Expected: prints the parity lines and `RESULT: PARITY PASS` (in QEMU this validates wiring/boot only, NOT the real SEMC timing — hardware-only, like USB host). If QEMU's second UART isn't stdio for this machine, reuse the serial setup from the existing EVKB QEMU sketch runs in `scratchpad`.

- [ ] **Step 2: Build-check the stock example still compiles**

```bash
sync
"$REPO"/extras/mimxrt1060_evkb/build_check.sh ARDUINO_MIMXRT1060_EVKB imxrt1060_evkb.ld 2>&1 | tail -5
```

Expected: passes (the EVKB core still builds the standard example).

- [ ] **Step 3: Document the SDRAM in README.md**

Add a section to `extras/mimxrt1060_evkb/README.md` (after the peripheral notes):

```markdown
## SDRAM (32 MB, EXTMEM)

The EVKB's 32 MB SDRAM (SEMC @ 0x80000000) is brought up by the core at boot
(`configure_external_ram()` -> `extram_semc_init()` in `semc.c`), so it works like
Teensy 4.1 PSRAM: the `EXTMEM` attribute places globals in SDRAM (zero-initialized
at boot) and `extmem_malloc()/free()/calloc()/realloc()` allocate from it;
`external_psram_size` reads 32.

Notes:
- SEMC runs at 132 MHz (PLL2 PFD2 / 3); PFD2 is shared with the boot flash and is
  not reprogrammed.
- `EXTMEM` globals with explicit initializers (`EXTMEM int x = 5;`) are zeroed, not
  value-initialized — same limitation as upstream Teensy 4.1 (copy-from-flash is an
  unfinished upstream TODO).
- Hardware-verified (full 32 MB memtest, 0 errors). QEMU backs this region with plain
  RAM and stubs SEMC, so emulation exercises the API but not the real SEMC timing.
```

- [ ] **Step 4: Final commit**

```bash
cd "$REPO" && git add extras/mimxrt1060_evkb/README.md extras/mimxrt1060_evkb/LIBRARY_SUPPORT.md
git commit -m "docs(EVKB): document 32 MB SDRAM EXTMEM support"
```

---

## Self-review

**Spec coverage:** Goal (EXTMEM parity + zeroed globals) → Tasks 3 (zero `.bss`, pool) + 1 (linker) + 4 (IS_EXTMEM) + 5 (verify). SEMC bring-up → Task 2. DQSMD=0, 132 MHz/PFD2, golden registers → Task 2 code. Boot-order/timing risk → resolved (busy-loop in Task 2). FlexSPI2-conflict removal → Task 3 (EVKB branch skips FlexSPI2). Testing (hardware/QEMU/build) → Tasks 5, 6. No gaps.

**Placeholders:** The only "fill this in" is Task 3 Step 3's `#else` branch = the *existing* Teensy 4.1 body moved verbatim (explicitly "do not alter") — not new code to invent. All new code is shown complete.

**Type/name consistency:** `extram_semc_init(void)` (semc.h/semc.c/startup.c), `IS_EXTMEM` (`>>28==8`), `extmem_smalloc_pool`, `external_psram_size`, `_extram_start`/`_extram_end`, `ERAM`/`.bss.extram` all consistent across tasks and match the existing core symbols.
