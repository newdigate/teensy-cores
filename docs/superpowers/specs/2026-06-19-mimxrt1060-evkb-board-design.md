# MIMXRT1060-EVKB Teensy Core/Board — Design

Date: 2026-06-19
Repo: PaulStoffregen/cores (this repo) — Teensy core library
Branch: claude/lucid-bohr-145339
Source hardware doc: `MIMXRT1060EVKBUM.pdf` (NXP, Rev. 0, Jan 2022)

## Goal

Add a new board, **NXP MIMXRT1060-EVKB**, that reuses the existing `teensy4`
core (i.MX RT1062, same chip family as Teensy 4.x). The board must:

1. Appear selectable in the Arduino IDE (via Teensyduino).
2. **Compile and link cleanly** for an ordinary sketch (e.g. Blink).
3. Ship a **physically accurate Arduino-header pin map** (D0–D15, A0–A5) and
   a correct `LED_BUILTIN`, derived from the EVKB user manual.
4. Use a linker memory map and FlexSPI boot config matched to the EVKB's
   on-board QSPI flash, giving the best chance of booting during the user's
   own hardware bring-up.

**Out of scope (user owns these):** flashing/upload to real hardware, on-board
hardware verification, SDRAM bring-up, USB device stack validation, and exact
peripheral-instance/ALT-mux verification for UART/SPI/I2C/PWM.

Success criterion: clean compile for the new board + a pin/ADC/LED map that is
correct by inspection against the UM.

## Confirmed design decisions

(From brainstorming with the user.)

- **Scope:** "Just the listing" — appears + compiles cleanly; user handles
  flashing and bring-up.
- **boards.txt delivery:** in-repo patch (a `boards.local.txt` snippet +
  instructions), applied by the user to their Teensyduino install. Do **not**
  edit the `/Applications/Teensyduino.app` bundle directly.
- **Pin-map fidelity:** accurate EVKB map now (real pad mapping + LED), not a
  Teensy-4.1 alias.
- **Sub-40-pin handling:** add `#if CORE_NUM_DIGITAL > 20` guards around the
  pin-20…39 cases in the shared fast-path switches / tables, mirroring the
  existing `> 40` / `> 46` idiom. EVKB stays a truthful 20-pin board; no
  fabricated pins. This is the only change to shared (non-board-specific) code.
- **Peripheral muxing:** pin *numbers* only for this pass. `Serial`/`SPI`/
  `Wire`/`analogWrite` target the correct Arduino header pins; underlying
  LPUART/LPSPI/LPI2C/FlexPWM instance + ALT mux selection is best-effort and
  flagged for hardware verification.

## Relevant hardware facts (from the UM)

| Item | Value | UM ref |
|------|-------|--------|
| MCU | i.MX RT1062 (Cortex-M7, 24 MHz xtal Y3, 32.768 kHz RTC Y4) | §2.1, §2.4 |
| Default boot flash | U24 **IS25WP064AJBLE**, QSPI NOR, 8 MB (64 Mbit), 1.8 V | §2.8.2 |
| Boot mode (default) | Internal boot, QSPI NOR, SW4[3:0]=`OFFOFFONOFF` | §2.2 |
| SDRAM | 32 MB on SEMC @ 0x80000000, 3.3 V, ≤166 MHz (not needed at boot) | §2.5 |
| User LED | D8 (green) on **GPIO_AD_B0_09** = Arduino header pin **D4** | §1.10, §2.12 |
| USB device | OTG1 on J48 (micro-AB) | §2.10 |
| Debug/upload | LPC4322 OpenSDA (DAPLink / CMSIS-DAP). **No HalfKay.** | §2.19 |

### EVKB Arduino-header pin map (the accurate map)

Core pin numbers follow Arduino convention: digital D0–D13 = pins 0–13,
analog A0–A5 = pins 14–19. **Total = 20 core pins.** D14/D15 (I2C) in the UM
are the same pads as A4/A5, i.e. pins 18/19 (no separate pins).

The core's existing Arduino-compat defaults already match the EVKB silkscreen
and need **no** EVKB override: `RX/TX = 0/1`, `SS/MOSI/MISO/SCK = 10/11/12/13`,
`SDA/SCL = 18/19 (A4/A5)`. Only `LED_BUILTIN` changes (13 → **4**).

| Pin | Arduino | RT1062 pad | UM table |
|-----|---------|------------|----------|
| 0  | D0 / RX  | GPIO_AD_B1_07 | J16 (T19) |
| 1  | D1 / TX  | GPIO_AD_B1_06 | J16 |
| 2  | D2 / INT0 | GPIO_AD_B0_11 | J16 |
| 3  | D3 / PWM | GPIO_AD_B1_08 | J16 |
| 4  | D4 / **LED** | GPIO_AD_B0_09 | J16 / §1.10 |
| 5  | D5 / PWM | GPIO_AD_B0_10 | J16 |
| 6  | D6 / PWM | GPIO_AD_B1_02 | J16 |
| 7  | D7 / PWM | GPIO_AD_B1_03 | J16 |
| 8  | D8       | GPIO_AD_B0_03 | J17 (T21) |
| 9  | D9 / PWM | GPIO_AD_B0_02 | J17 |
| 10 | D10 / SPI_CS | GPIO_SD_B0_01 | J17 |
| 11 | D11 / SPI_MOSI | GPIO_SD_B0_02 | J17 |
| 12 | D12 / SPI_MISO | GPIO_SD_B0_03 | J17 |
| 13 | D13 / SPI_CLK | GPIO_SD_B0_00 | J17 |
| 14 | A0 / ADC0 | GPIO_AD_B1_10 | J33 (T20) |
| 15 | A1 / ADC1 | GPIO_AD_B1_11 | J33 |
| 16 | A2 / ADC2 | GPIO_AD_B1_04 | J33 |
| 17 | A3 / ADC3 | GPIO_AD_B1_05 | J33 |
| 18 | A4 / ADC4 / SDA | GPIO_AD_B1_01 | J33 / J17 |
| 19 | A5 / ADC5 / SCL | GPIO_AD_B1_00 | J33 / J17 |

## Approach (chosen)

Add `ARDUINO_MIMXRT1060_EVKB` as a first-class board variant of the `teensy4`
core, alongside `ARDUINO_TEENSY40` / `_TEENSY41` / `_TEENSY_MICROMOD`.

Two kinds of change:

- **Chip-level (treat EVKB like Teensy 4.1):** for files whose board guards
  select RT1062 peripheral/chip config that is identical on the EVKB (USB,
  EEPROM-in-flash, extmem, serial peripheral instances, startup/clocks), add
  `|| defined(ARDUINO_MIMXRT1060_EVKB)` to the Teensy-4.1 branch.
- **Board-specific (new EVKB data):** pin map, LED, pin counts, linker memory
  map, and FlexSPI boot config get their own EVKB values.

**Harvest, don't invent:** every EVKB pad already appears in the existing
Teensy tables (`core_pins.h`, `digital.c`, `pwm.c`, `analog.c`), where its GPIO
fast-instance/bit, IOMUXC mux/pad register, ADC channel, and FlexPWM/QuadTimer
submodule are already encoded. EVKB tables are built by copying each pad's
existing register facts, then cross-checking against `imxrt.h`.

### Alternatives rejected
- **Full functional port** (verified muxing, SDRAM, USB): far beyond
  "appears + compiles."
- **Thin alias to Teensy 4.1** (reuse T4.1 pin universe): pin numbers would be
  physically wrong for the EVKB headers — fails the accuracy requirement.

## File-by-file change plan

### New files
- `teensy4/imxrt1060_evkb.ld` — linker script, cloned from
  `imxrt1062_t41.ld`: FLASH 8 MB @ 0x60000000; ITCM/DTCM/OCRAM as RT1062; plus
  a defined-but-unused 32 MB SDRAM region @ 0x80000000 (SEMC) for later.
- `extras/mimxrt1060_evkb/boards.local.txt` — the board stanza
  (`mimxrt1060evkb.*`): `name`, `build.board=MIMXRT1060_EVKB`,
  `build.core=teensy4`, `build.mcu=imxrt1062`,
  `build.flags.ld=... -T{build.core.path}/imxrt1060_evkb.ld`, and the
  USB/Speed/Opt/Keys menus copied from the Teensy 4.x pattern. No HalfKay
  upload tool wired.
- `extras/mimxrt1060_evkb/README.md` — install instructions (where to place
  `boards.local.txt`; fallback = append the stanza to `boards.txt`), the
  documented flashing options (DAPLink drag-drop / pyOCD / OpenOCD via OpenSDA),
  and the "best-effort / verify on hardware" caveats.

### Board-specific data (new `#elif defined(ARDUINO_MIMXRT1060_EVKB)` blocks)
- `teensy4/core_pins.h` — new ~250-line per-pin block: `CORE_NUM_TOTAL_PINS=20`,
  `CORE_NUM_DIGITAL=20`, `CORE_NUM_ANALOG=6`, and `CORE_PIN0..19` macros
  (`_BIT`, `_BITMASK`, `_PORTREG/SET/CLEAR/TOGGLE`, `_PINREG`, `_CONFIG`,
  `_PADCONFIG`) for the 20 pads above, harvested from existing tables.
- `teensy4/pins_arduino.h` — EVKB block: `NUM_DIGITAL_PINS=20`,
  `NUM_ANALOG_INPUTS=6`, `LED_BUILTIN=4` (override the global 13),
  `analogInputToDigitalPin`/`digitalPinHasPWM` macros. (SS/MOSI/MISO/SCK,
  SDA/SCL, RX/TX defaults already correct — no change.)
- `teensy4/analog.c` — EVKB `pin_to_channel[]` (20 entries): A0–A5 → their ADC
  channels (harvested), digital-only pins → invalid/unused marker.
- `teensy4/pwm.c` — EVKB `pwm_pin_info[]` (20 entries). The shared
  unconditional entries (pins 0–33, Teensy pads) must be guarded out for the
  EVKB; EVKB entries use the correct FlexPWM/QuadTimer submodule per pad where
  determinable, null `{0,M(1,0),0,0}` otherwise (best-effort, per "pin numbers
  only").

### Shared-code guard changes (sub-20-pin support)
- `teensy4/core_pins.h` — wrap the pin-20…39 cases in the fast-path inline
  switches (`digitalWriteFast` set/clear, `digitalToggleFast`,
  `digitalReadFast`, and any other unconditional 0–39 switch) with
  `#if CORE_NUM_DIGITAL > 20 … #endif`. Existing boards (40/46/55 pins) are
  unaffected.
- `teensy4/digital.c` — wrap `digital_pin_to_info_PGM[]` entries 20…39 with
  `#if CORE_NUM_DIGITAL > 20`.

### Chip-level guards (add EVKB to the Teensy-4.1 branch so it compiles)
For each file below, add `|| defined(ARDUINO_MIMXRT1060_EVKB)` to the relevant
`defined(ARDUINO_TEENSY41)` guard (chip config is identical):
- `teensy4/bootdata.c` — **and** add a board-guarded `FlexSPI_NOR_Config`
  tuned for the IS25WP064A (from NXP's known-good EVKB values); IVT/BootData
  unchanged; DCD = 0.
- `teensy4/usb_desc.c` — USB descriptors / product strings.
- `teensy4/eeprom.c` — EEPROM-in-flash emulation parameters.
- `teensy4/extmem.c` — external memory init (PSRAM auto-detect; harmless).
- `teensy4/startup.c` — startup/clock config branch.
- `teensy4/HardwareSerial.cpp`, `HardwareSerial.h`, `HardwareSerial1/2/4/5/8.cpp`
  — so the LPUART instances compile under the EVKB define (pin accuracy is
  best-effort/documented; OpenSDA debug VCOM is LPUART1, separate from the
  Arduino-header D0/D1 = LPUART3 — note in README).
- `teensy4/smalloc.h` — heap region selection, if board-guarded.
- `teensy4/Makefile` — add a commented EVKB option block
  (`MCU_DEF=ARDUINO_MIMXRT1060_EVKB`, `MCU_LD=imxrt1060_evkb.ld`) mirroring the
  existing Teensy 4.0/4.1 comment blocks.

The exact guard list will be confirmed by grepping
`ARDUINO_TEENSY41` across `teensy4/` during implementation; every hit must
either gain the EVKB define or be intentionally left Teensy-only.

## Testing / verification

"Compiles cleanly" is the gate. Verified by building the core for the new board
with the Teensyduino-bundled toolchain, without needing the IDE or hardware:

1. Build the core + a Blink sketch via `teensy4/Makefile` with
   `MCU_DEF=ARDUINO_MIMXRT1060_EVKB`, `MCU_LD=imxrt1060_evkb.ld`, using
   `arm-none-eabi-gcc` from `/Applications/Teensyduino.app/.../arm/bin`.
   Expect: clean compile + link, `.hex`/`.elf` produced, no undefined
   `CORE_PINxx` / missing-symbol errors.
2. Confirm the four existing boards still build (regression on the shared
   `> 20` guard changes): Teensy 4.0, 4.1, MicroMod.
3. Spot-check the generated map: `LED_BUILTIN==4`, `NUM_DIGITAL_PINS==20`,
   `NUM_ANALOG_INPUTS==6`, and `digitalPinToBitMask(4)` resolves to the
   GPIO_AD_B0_09 bit.

No on-hardware test (per scope). Flashing instructions are documented for the
user in `extras/mimxrt1060_evkb/README.md`.

## Risks / notes
- **boards.local.txt new-board support** varies by Arduino IDE version; the
  README gives the append-to-`boards.txt` fallback, and verification uses the
  Makefile path (toolchain-only) so listing support doesn't block the build
  gate.
- **FlexSPI config** for the IS25WP064A is correct-by-reference but unverified
  on hardware (user's bring-up).
- **Peripheral pin accuracy** (which LPUART/LPSPI/LPI2C/FlexPWM instance, ALT
  mux) is best-effort this pass and flagged for hardware verification.
- **SD_B0 GPIO bits** (pins 10–13) must be cross-checked against `imxrt.h`
  during implementation rather than assumed.
