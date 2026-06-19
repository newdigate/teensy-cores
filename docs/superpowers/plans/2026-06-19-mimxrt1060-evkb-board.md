# MIMXRT1060-EVKB Board Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a new board, **NXP MIMXRT1060-EVKB**, to the Teensy `teensy4` core (i.MX RT1062) that appears in the Arduino IDE and compiles/links cleanly, with a physically accurate 20-pin Arduino-header map and correct `LED_BUILTIN`.

**Architecture:** New `ARDUINO_MIMXRT1060_EVKB` board variant alongside Teensy 4.0/4.1/MicroMod. Board-specific data (20-pin map, LED, pin counts, linker, FlexSPI flash size) gets EVKB values; chip-level config identical to the RT1062 is shared by adding the EVKB define to the Teensy-4.1 branches. Every EVKB pad's register facts are harvested from the existing core tables. The core's hardcoded ≥40-pin assumption is relaxed with additive `#if CORE_NUM_DIGITAL > 20` guards.

**Tech Stack:** C/C++, GNU Arm Embedded toolchain (`arm-none-eabi-gcc` 11.3 from Teensyduino), GNU make, Teensyduino `boards.txt`.

**Spec:** `docs/superpowers/specs/2026-06-19-mimxrt1060-evkb-board-design.md`

---

## Reference data (used by multiple tasks)

**EVKB pin map** (core pin → RT1062 pad → fast-GPIO instance/bit → ADC1 ch → PWM):

| Pin | Arduino | Pad | GPIO | BIT | ADC ch | PWM submodule |
|----:|---------|-----|:----:|:---:|:------:|---------------|
| 0  | D0/RX  | AD_B1_07 | 6 | 23 | 12  | — |
| 1  | D1/TX  | AD_B1_06 | 6 | 22 | 11  | — |
| 2  | D2     | AD_B0_11 | 6 | 11 | 255 | — |
| 3  | D3     | AD_B1_08 | 6 | 24 | 13  | FlexPWM4_0_A `{1,M(4,0),1,1}` |
| 4  | D4/LED | AD_B0_09 | 6 | 9  | 255 | — |
| 5  | D5     | AD_B0_10 | 6 | 10 | 255 | — |
| 6  | D6     | AD_B1_02 | 6 | 18 | 7   | QuadTimer3_2 `{2,M(3,2),0,1}` |
| 7  | D7     | AD_B1_03 | 6 | 19 | 8   | QuadTimer3_3 `{2,M(3,3),0,1}` |
| 8  | D8     | AD_B0_03 | 6 | 3  | 255 | FlexPWM1_1_X `{1,M(1,1),0,4}` |
| 9  | D9     | AD_B0_02 | 6 | 2  | 255 | FlexPWM1_0_X `{1,M(1,0),0,4}` |
| 10 | D10/CS | SD_B0_01 | 8 | 13 | 255 | FlexPWM1_0_B `{1,M(1,0),2,1}` |
| 11 | D11/MOSI | SD_B0_02 | 8 | 14 | 255 | FlexPWM1_1_A `{1,M(1,1),1,1}` |
| 12 | D12/MISO | SD_B0_03 | 8 | 15 | 255 | FlexPWM1_1_B `{1,M(1,1),2,1}` |
| 13 | D13/SCK | SD_B0_00 | 8 | 12 | 255 | FlexPWM1_0_A `{1,M(1,0),1,1}` |
| 14 | A0     | AD_B1_10 | 6 | 26 | 15  | — |
| 15 | A1     | AD_B1_11 | 6 | 27 | 0   | — |
| 16 | A2     | AD_B1_04 | 6 | 20 | 9   | — |
| 17 | A3     | AD_B1_05 | 6 | 21 | 10  | — |
| 18 | A4/SDA | AD_B1_01 | 6 | 17 | 6   | QuadTimer3_1 `{2,M(3,1),0,1}` |
| 19 | A5/SCL | AD_B1_00 | 6 | 16 | 5   | QuadTimer3_0 `{2,M(3,0),0,1}` |

Pads with no PWM/ADC use `255` (ADC) / `{0, M(1, 0), 0, 0}` (PWM). PWM is best-effort (spec: "pin numbers only").

**Common build variables** (used in verification commands; paste at the top of a shell session):

```bash
export ARDUINOPATH=/Applications/Teensyduino.app/Contents/Java
export GCC="$ARDUINOPATH/hardware/tools/arm/bin/arm-none-eabi-gcc"
export GXX="$ARDUINOPATH/hardware/tools/arm/bin/arm-none-eabi-g++"
export CPU="-mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16 -mthumb"
export DEFS="-D__IMXRT1062__ -DARDUINO_MIMXRT1060_EVKB -DF_CPU=600000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DARDUINO=10813 -DTEENSYDUINO=159"
export CXX_ONLY="-std=gnu++17 -felide-constructors -fno-exceptions -fpermissive -fno-rtti"
# Single-file compile checks (run from the teensy4/ dir):
#   C   : $GCC -c $CPU $DEFS -I. <file>.c   -o /tmp/o.o
#   C++ : $GXX -c $CPU $DEFS $CXX_ONLY -I. <file>.cpp -o /tmp/o.o
```

All paths below are relative to the repo root unless absolute.

---

### Task 1: Build harness + baseline

**Files:**
- Create: `extras/mimxrt1060_evkb/build_check.sh`
- Modify: `.gitignore` (repo root)

- [ ] **Step 1: Write the build-check script**

Create `extras/mimxrt1060_evkb/build_check.sh`:

```bash
#!/usr/bin/env bash
# Compile+link the teensy4 core for a given board, to verify a clean build.
# Usage: build_check.sh <MCU_DEF> <MCU_LD>
#   e.g. build_check.sh ARDUINO_MIMXRT1060_EVKB imxrt1060_evkb.ld
set -euo pipefail
ARDUINOPATH="${ARDUINOPATH:-/Applications/Teensyduino.app/Contents/Java}"
DEF="$1"; LD="$2"
HERE="$(cd "$(dirname "$0")" && pwd)"
CORE="$HERE/../../teensy4"
cd "$CORE"
# the cross-gcc finds -larm_cortexM7lfsp_math via LIBRARY_PATH
export LIBRARY_PATH="$ARDUINOPATH/hardware/tools/arm/arm-none-eabi/lib"
# minimal sketch supplies setup()/loop() so main.cpp's main() links
cat > _buildcheck_sketch.cpp <<'EOF'
#include <Arduino.h>
void setup() { pinMode(LED_BUILTIN, OUTPUT); }
void loop() { digitalWriteFast(LED_BUILTIN, HIGH); delay(100); digitalWriteFast(LED_BUILTIN, LOW); delay(100); }
EOF
trap 'rm -f _buildcheck_sketch.cpp *.o *.d main.elf' EXIT
make -s ARDUINOPATH="$ARDUINOPATH" MCU=IMXRT1062 MCU_DEF="$DEF" MCU_LD="$LD" main.elf
echo "BUILD OK: $DEF / $LD"
```

- [ ] **Step 2: Make it executable and run the baseline (Teensy 4.1)**

Run:
```bash
chmod +x extras/mimxrt1060_evkb/build_check.sh
extras/mimxrt1060_evkb/build_check.sh ARDUINO_TEENSY41 imxrt1062_t41.ld
```
Expected: ends with `BUILD OK: ARDUINO_TEENSY41 / imxrt1062_t41.ld`. (If linking fails to find `libarm_cortexM7lfsp_math`, confirm the `LIBRARY_PATH` dir exists.)

- [ ] **Step 3: Ignore build artifacts**

Add to `.gitignore` (repo root):
```
teensy4/_buildcheck_sketch.cpp
teensy4/main.elf
teensy4/main.hex
teensy4/*.o
teensy4/*.d
```

- [ ] **Step 4: Commit**

```bash
git add extras/mimxrt1060_evkb/build_check.sh .gitignore
git commit -m "Add EVKB build-check harness; verify Teensy 4.1 baseline"
```

---

### Task 2: Linker script

**Files:**
- Create: `teensy4/imxrt1060_evkb.ld` (clone of `teensy4/imxrt1062_t41.ld`)

- [ ] **Step 1: Clone the Teensy 4.1 linker script**

Run:
```bash
cp teensy4/imxrt1062_t41.ld teensy4/imxrt1060_evkb.ld
```

- [ ] **Step 2: Add an EVKB header comment**

Insert these comment lines at the very top of `teensy4/imxrt1060_evkb.ld` (before `MEMORY`):
```
/* NXP MIMXRT1060-EVKB - i.MX RT1062, 8 MB QSPI NOR (IS25WP064A) on FlexSPI.
 * Memory map identical to Teensy 4.1 (8 MB flash). NOTE: the board also has
 * 32 MB SDRAM on SEMC @ 0x80000000; SDRAM bring-up is deferred. The ERAM
 * region below is left as the Teensy-4.1 FlexSPI2 layout for compile
 * compatibility with extmem.c and is unused unless code places .externalram. */
```

- [ ] **Step 3: Verify the script links (using an already-working board define)**

Run:
```bash
extras/mimxrt1060_evkb/build_check.sh ARDUINO_TEENSY41 imxrt1060_evkb.ld
```
Expected: `BUILD OK: ARDUINO_TEENSY41 / imxrt1060_evkb.ld` (proves the new `.ld` parses and links).

- [ ] **Step 4: Commit**

```bash
git add teensy4/imxrt1060_evkb.ld
git commit -m "Add EVKB linker script (8MB flash, cloned from Teensy 4.1)"
```

---

### Task 3: FlexSPI flash size in bootdata.c

**Files:**
- Modify: `teensy4/bootdata.c:96-99`

- [ ] **Step 1: Add the EVKB flash-size branch**

In `teensy4/bootdata.c`, find:
```c
#elif defined(ARDUINO_TEENSY41)
	0x00800000,		// sflashA1Size			0x50
#elif defined(ARDUINO_TEENSY_MICROMOD)
	0x01000000,		// sflashA1Size			0x50
```
Insert a new branch immediately after the Teensy 4.1 one (EVKB IS25WP064A = 8 MB, same value):
```c
#elif defined(ARDUINO_TEENSY41)
	0x00800000,		// sflashA1Size			0x50
#elif defined(ARDUINO_MIMXRT1060_EVKB)
	0x00800000,		// sflashA1Size 8MB (IS25WP064A QSPI NOR)
#elif defined(ARDUINO_TEENSY_MICROMOD)
	0x01000000,		// sflashA1Size			0x50
```

- [ ] **Step 2: Verify bootdata.c compiles standalone for the EVKB**

Run (from `teensy4/`):
```bash
cd teensy4 && $GCC -c $CPU $DEFS -I. bootdata.c -o /tmp/bootdata.o && echo COMPILE_OK; cd ..
```
Expected: `COMPILE_OK` (no "FlexSPI_NOR_Config" initializer errors → the `#elif` chain resolved).

- [ ] **Step 3: Commit**

```bash
git add teensy4/bootdata.c
git commit -m "bootdata: add EVKB FlexSPI flash size (8MB IS25WP064A)"
```

---

### Task 4: pins_arduino.h EVKB block

**Files:**
- Modify: `teensy4/pins_arduino.h:76` (LED_BUILTIN), `:96-105` (pin counts), `:110-121` (pin macros)

- [ ] **Step 1: Make LED_BUILTIN board-aware**

In `teensy4/pins_arduino.h`, replace the single line `#define LED_BUILTIN   (13)` with:
```c
#if defined(ARDUINO_MIMXRT1060_EVKB)
#define LED_BUILTIN   (4)    // user LED D8 = GPIO_AD_B0_09 = Arduino D4
#else
#define LED_BUILTIN   (13)
#endif
```

- [ ] **Step 2: Add EVKB pin counts**

In the `#if defined(ARDUINO_TEENSY40) ... #endif` counts block, add an `#elif` before `#endif`:
```c
#elif defined(ARDUINO_MIMXRT1060_EVKB)
  #define NUM_DIGITAL_PINS  20
  #define NUM_ANALOG_INPUTS 6
#endif
```

- [ ] **Step 3: Add EVKB analog/PWM helper macros**

In the `#if defined(__IMXRT1062__) && defined(ARDUINO_TEENSY40) ... #endif` macro block, add an `#elif` before the final `#endif` of that block:
```c
#elif defined(__IMXRT1062__) && defined(ARDUINO_MIMXRT1060_EVKB)
  #define analogInputToDigitalPin(p) (((p) <= 5) ? (p) + 14 : (((p) >= 14 && (p) <= 19) ? (p) : -1))
  #define digitalPinHasPWM(p) ((p) == 3 || ((p) >= 6 && (p) <= 13) || (p) == 18 || (p) == 19)
```

- [ ] **Step 4: Verify pins_arduino.h preprocesses with EVKB values**

Run (from `teensy4/`):
```bash
cd teensy4 && printf '#include "pins_arduino.h"\nint led=LED_BUILTIN; int nd=NUM_DIGITAL_PINS; int na=NUM_ANALOG_INPUTS;\n' > /tmp/t_pa.c \
  && $GCC -E $DEFS -I. /tmp/t_pa.c | grep -E "int led=|int nd=|int na=" ; cd ..
```
Expected: `int led=(4)`, `int nd=20`, `int na=6`.

- [ ] **Step 5: Commit**

```bash
git add teensy4/pins_arduino.h
git commit -m "pins_arduino: add EVKB block (LED=D4, 20 digital, 6 analog)"
```

---

### Task 5: core_pins.h — EVKB pin macros + sub-20-pin guards

**Files:**
- Modify: `teensy4/core_pins.h` (add `#elif` board block before `#endif // __IMXRT1062__` at line ~1775; add 4 guard regions in the fast-path switches)

- [ ] **Step 1: Add the EVKB per-pin macro block**

In `teensy4/core_pins.h`, immediately before the line `#endif // __IMXRT1062__` (the one that closes the board `#if/#elif` chain, ~line 1775), insert this complete block:

```c
#elif defined(__IMXRT1062__) && defined(ARDUINO_MIMXRT1060_EVKB)

#define CORE_NUM_TOTAL_PINS	20
#define CORE_NUM_DIGITAL	20
#define CORE_NUM_INTERRUPT	20
#define CORE_NUM_ANALOG		6
#define CORE_NUM_PWM		11

#define CORE_PIN0_BIT		23
#define CORE_PIN1_BIT		22
#define CORE_PIN2_BIT		11
#define CORE_PIN3_BIT		24
#define CORE_PIN4_BIT		9
#define CORE_PIN5_BIT		10
#define CORE_PIN6_BIT		18
#define CORE_PIN7_BIT		19
#define CORE_PIN8_BIT		3
#define CORE_PIN9_BIT		2
#define CORE_PIN10_BIT		13
#define CORE_PIN11_BIT		14
#define CORE_PIN12_BIT		15
#define CORE_PIN13_BIT		12
#define CORE_PIN14_BIT		26
#define CORE_PIN15_BIT		27
#define CORE_PIN16_BIT		20
#define CORE_PIN17_BIT		21
#define CORE_PIN18_BIT		17
#define CORE_PIN19_BIT		16

#define CORE_PIN0_BITMASK	(1<<(CORE_PIN0_BIT))
#define CORE_PIN1_BITMASK	(1<<(CORE_PIN1_BIT))
#define CORE_PIN2_BITMASK	(1<<(CORE_PIN2_BIT))
#define CORE_PIN3_BITMASK	(1<<(CORE_PIN3_BIT))
#define CORE_PIN4_BITMASK	(1<<(CORE_PIN4_BIT))
#define CORE_PIN5_BITMASK	(1<<(CORE_PIN5_BIT))
#define CORE_PIN6_BITMASK	(1<<(CORE_PIN6_BIT))
#define CORE_PIN7_BITMASK	(1<<(CORE_PIN7_BIT))
#define CORE_PIN8_BITMASK	(1<<(CORE_PIN8_BIT))
#define CORE_PIN9_BITMASK	(1<<(CORE_PIN9_BIT))
#define CORE_PIN10_BITMASK	(1<<(CORE_PIN10_BIT))
#define CORE_PIN11_BITMASK	(1<<(CORE_PIN11_BIT))
#define CORE_PIN12_BITMASK	(1<<(CORE_PIN12_BIT))
#define CORE_PIN13_BITMASK	(1<<(CORE_PIN13_BIT))
#define CORE_PIN14_BITMASK	(1<<(CORE_PIN14_BIT))
#define CORE_PIN15_BITMASK	(1<<(CORE_PIN15_BIT))
#define CORE_PIN16_BITMASK	(1<<(CORE_PIN16_BIT))
#define CORE_PIN17_BITMASK	(1<<(CORE_PIN17_BIT))
#define CORE_PIN18_BITMASK	(1<<(CORE_PIN18_BIT))
#define CORE_PIN19_BITMASK	(1<<(CORE_PIN19_BIT))

// Fast GPIO: AD_B0/AD_B1 pads -> GPIO6 ; SD_B0 pads -> GPIO8
#define CORE_PIN0_PORTREG	GPIO6_DR
#define CORE_PIN1_PORTREG	GPIO6_DR
#define CORE_PIN2_PORTREG	GPIO6_DR
#define CORE_PIN3_PORTREG	GPIO6_DR
#define CORE_PIN4_PORTREG	GPIO6_DR
#define CORE_PIN5_PORTREG	GPIO6_DR
#define CORE_PIN6_PORTREG	GPIO6_DR
#define CORE_PIN7_PORTREG	GPIO6_DR
#define CORE_PIN8_PORTREG	GPIO6_DR
#define CORE_PIN9_PORTREG	GPIO6_DR
#define CORE_PIN10_PORTREG	GPIO8_DR
#define CORE_PIN11_PORTREG	GPIO8_DR
#define CORE_PIN12_PORTREG	GPIO8_DR
#define CORE_PIN13_PORTREG	GPIO8_DR
#define CORE_PIN14_PORTREG	GPIO6_DR
#define CORE_PIN15_PORTREG	GPIO6_DR
#define CORE_PIN16_PORTREG	GPIO6_DR
#define CORE_PIN17_PORTREG	GPIO6_DR
#define CORE_PIN18_PORTREG	GPIO6_DR
#define CORE_PIN19_PORTREG	GPIO6_DR

#define CORE_PIN0_PORTSET	GPIO6_DR_SET
#define CORE_PIN1_PORTSET	GPIO6_DR_SET
#define CORE_PIN2_PORTSET	GPIO6_DR_SET
#define CORE_PIN3_PORTSET	GPIO6_DR_SET
#define CORE_PIN4_PORTSET	GPIO6_DR_SET
#define CORE_PIN5_PORTSET	GPIO6_DR_SET
#define CORE_PIN6_PORTSET	GPIO6_DR_SET
#define CORE_PIN7_PORTSET	GPIO6_DR_SET
#define CORE_PIN8_PORTSET	GPIO6_DR_SET
#define CORE_PIN9_PORTSET	GPIO6_DR_SET
#define CORE_PIN10_PORTSET	GPIO8_DR_SET
#define CORE_PIN11_PORTSET	GPIO8_DR_SET
#define CORE_PIN12_PORTSET	GPIO8_DR_SET
#define CORE_PIN13_PORTSET	GPIO8_DR_SET
#define CORE_PIN14_PORTSET	GPIO6_DR_SET
#define CORE_PIN15_PORTSET	GPIO6_DR_SET
#define CORE_PIN16_PORTSET	GPIO6_DR_SET
#define CORE_PIN17_PORTSET	GPIO6_DR_SET
#define CORE_PIN18_PORTSET	GPIO6_DR_SET
#define CORE_PIN19_PORTSET	GPIO6_DR_SET

#define CORE_PIN0_PORTCLEAR	GPIO6_DR_CLEAR
#define CORE_PIN1_PORTCLEAR	GPIO6_DR_CLEAR
#define CORE_PIN2_PORTCLEAR	GPIO6_DR_CLEAR
#define CORE_PIN3_PORTCLEAR	GPIO6_DR_CLEAR
#define CORE_PIN4_PORTCLEAR	GPIO6_DR_CLEAR
#define CORE_PIN5_PORTCLEAR	GPIO6_DR_CLEAR
#define CORE_PIN6_PORTCLEAR	GPIO6_DR_CLEAR
#define CORE_PIN7_PORTCLEAR	GPIO6_DR_CLEAR
#define CORE_PIN8_PORTCLEAR	GPIO6_DR_CLEAR
#define CORE_PIN9_PORTCLEAR	GPIO6_DR_CLEAR
#define CORE_PIN10_PORTCLEAR	GPIO8_DR_CLEAR
#define CORE_PIN11_PORTCLEAR	GPIO8_DR_CLEAR
#define CORE_PIN12_PORTCLEAR	GPIO8_DR_CLEAR
#define CORE_PIN13_PORTCLEAR	GPIO8_DR_CLEAR
#define CORE_PIN14_PORTCLEAR	GPIO6_DR_CLEAR
#define CORE_PIN15_PORTCLEAR	GPIO6_DR_CLEAR
#define CORE_PIN16_PORTCLEAR	GPIO6_DR_CLEAR
#define CORE_PIN17_PORTCLEAR	GPIO6_DR_CLEAR
#define CORE_PIN18_PORTCLEAR	GPIO6_DR_CLEAR
#define CORE_PIN19_PORTCLEAR	GPIO6_DR_CLEAR

#define CORE_PIN0_PORTTOGGLE	GPIO6_DR_TOGGLE
#define CORE_PIN1_PORTTOGGLE	GPIO6_DR_TOGGLE
#define CORE_PIN2_PORTTOGGLE	GPIO6_DR_TOGGLE
#define CORE_PIN3_PORTTOGGLE	GPIO6_DR_TOGGLE
#define CORE_PIN4_PORTTOGGLE	GPIO6_DR_TOGGLE
#define CORE_PIN5_PORTTOGGLE	GPIO6_DR_TOGGLE
#define CORE_PIN6_PORTTOGGLE	GPIO6_DR_TOGGLE
#define CORE_PIN7_PORTTOGGLE	GPIO6_DR_TOGGLE
#define CORE_PIN8_PORTTOGGLE	GPIO6_DR_TOGGLE
#define CORE_PIN9_PORTTOGGLE	GPIO6_DR_TOGGLE
#define CORE_PIN10_PORTTOGGLE	GPIO8_DR_TOGGLE
#define CORE_PIN11_PORTTOGGLE	GPIO8_DR_TOGGLE
#define CORE_PIN12_PORTTOGGLE	GPIO8_DR_TOGGLE
#define CORE_PIN13_PORTTOGGLE	GPIO8_DR_TOGGLE
#define CORE_PIN14_PORTTOGGLE	GPIO6_DR_TOGGLE
#define CORE_PIN15_PORTTOGGLE	GPIO6_DR_TOGGLE
#define CORE_PIN16_PORTTOGGLE	GPIO6_DR_TOGGLE
#define CORE_PIN17_PORTTOGGLE	GPIO6_DR_TOGGLE
#define CORE_PIN18_PORTTOGGLE	GPIO6_DR_TOGGLE
#define CORE_PIN19_PORTTOGGLE	GPIO6_DR_TOGGLE

#define CORE_PIN0_DDRREG	GPIO6_GDIR
#define CORE_PIN1_DDRREG	GPIO6_GDIR
#define CORE_PIN2_DDRREG	GPIO6_GDIR
#define CORE_PIN3_DDRREG	GPIO6_GDIR
#define CORE_PIN4_DDRREG	GPIO6_GDIR
#define CORE_PIN5_DDRREG	GPIO6_GDIR
#define CORE_PIN6_DDRREG	GPIO6_GDIR
#define CORE_PIN7_DDRREG	GPIO6_GDIR
#define CORE_PIN8_DDRREG	GPIO6_GDIR
#define CORE_PIN9_DDRREG	GPIO6_GDIR
#define CORE_PIN10_DDRREG	GPIO8_GDIR
#define CORE_PIN11_DDRREG	GPIO8_GDIR
#define CORE_PIN12_DDRREG	GPIO8_GDIR
#define CORE_PIN13_DDRREG	GPIO8_GDIR
#define CORE_PIN14_DDRREG	GPIO6_GDIR
#define CORE_PIN15_DDRREG	GPIO6_GDIR
#define CORE_PIN16_DDRREG	GPIO6_GDIR
#define CORE_PIN17_DDRREG	GPIO6_GDIR
#define CORE_PIN18_DDRREG	GPIO6_GDIR
#define CORE_PIN19_DDRREG	GPIO6_GDIR

#define CORE_PIN0_PINREG	GPIO6_PSR
#define CORE_PIN1_PINREG	GPIO6_PSR
#define CORE_PIN2_PINREG	GPIO6_PSR
#define CORE_PIN3_PINREG	GPIO6_PSR
#define CORE_PIN4_PINREG	GPIO6_PSR
#define CORE_PIN5_PINREG	GPIO6_PSR
#define CORE_PIN6_PINREG	GPIO6_PSR
#define CORE_PIN7_PINREG	GPIO6_PSR
#define CORE_PIN8_PINREG	GPIO6_PSR
#define CORE_PIN9_PINREG	GPIO6_PSR
#define CORE_PIN10_PINREG	GPIO8_PSR
#define CORE_PIN11_PINREG	GPIO8_PSR
#define CORE_PIN12_PINREG	GPIO8_PSR
#define CORE_PIN13_PINREG	GPIO8_PSR
#define CORE_PIN14_PINREG	GPIO6_PSR
#define CORE_PIN15_PINREG	GPIO6_PSR
#define CORE_PIN16_PINREG	GPIO6_PSR
#define CORE_PIN17_PINREG	GPIO6_PSR
#define CORE_PIN18_PINREG	GPIO6_PSR
#define CORE_PIN19_PINREG	GPIO6_PSR

#define CORE_PIN0_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_07
#define CORE_PIN1_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_06
#define CORE_PIN2_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_11
#define CORE_PIN3_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_08
#define CORE_PIN4_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_09
#define CORE_PIN5_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_10
#define CORE_PIN6_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_02
#define CORE_PIN7_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_03
#define CORE_PIN8_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_03
#define CORE_PIN9_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_02
#define CORE_PIN10_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_SD_B0_01
#define CORE_PIN11_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_SD_B0_02
#define CORE_PIN12_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_SD_B0_03
#define CORE_PIN13_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_SD_B0_00
#define CORE_PIN14_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_10
#define CORE_PIN15_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_11
#define CORE_PIN16_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_04
#define CORE_PIN17_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_05
#define CORE_PIN18_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_01
#define CORE_PIN19_CONFIG	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_00

#define CORE_PIN0_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_07
#define CORE_PIN1_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_06
#define CORE_PIN2_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_11
#define CORE_PIN3_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_08
#define CORE_PIN4_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_09
#define CORE_PIN5_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_10
#define CORE_PIN6_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_02
#define CORE_PIN7_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_03
#define CORE_PIN8_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_03
#define CORE_PIN9_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_02
#define CORE_PIN10_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_SD_B0_01
#define CORE_PIN11_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_SD_B0_02
#define CORE_PIN12_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_SD_B0_03
#define CORE_PIN13_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_SD_B0_00
#define CORE_PIN14_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_10
#define CORE_PIN15_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_11
#define CORE_PIN16_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_04
#define CORE_PIN17_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_05
#define CORE_PIN18_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_01
#define CORE_PIN19_PADCONFIG	IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_00
```

- [ ] **Step 2: Add the four `> 20` guard regions in the fast-path switches**

There are exactly **4** unconditional `} else if (pin == 20) … pin == 39 …` ladders, each immediately followed by `#if CORE_NUM_DIGITAL > 40`. For **each** of the 4 regions:
1. Insert a line `#if CORE_NUM_DIGITAL > 20` *before* the `} else if (pin == 20) {` line.
2. Insert a line `#endif` *after* the closing `}` of the `pin == 39` case (i.e. on the blank line directly before the existing `#if CORE_NUM_DIGITAL > 40`).

Locate them with:
```bash
grep -n "pin == 20\|pin == 39\|CORE_NUM_DIGITAL > 40" teensy4/core_pins.h
```
Expected: 4 triples (`pin == 20`, `pin == 39`, `CORE_NUM_DIGITAL > 40`). Example transformation (apply to all 4):
```c
			} else if (pin == 19) {
				CORE_PIN19_PORTSET = CORE_PIN19_BITMASK;
#if CORE_NUM_DIGITAL > 20
			} else if (pin == 20) {
				CORE_PIN20_PORTSET = CORE_PIN20_BITMASK;
				/* ... pins 21..38 unchanged ... */
			} else if (pin == 39) {
				CORE_PIN39_PORTSET = CORE_PIN39_BITMASK;
#endif
#if CORE_NUM_DIGITAL > 40
```

- [ ] **Step 3: Verify core_pins.h compiles for the EVKB**

Run (from `teensy4/`):
```bash
cd teensy4 && printf '#include "core_pins.h"\n' > /tmp/t_cp.cpp \
  && $GXX -c $CPU $DEFS $CXX_ONLY -I. /tmp/t_cp.cpp -o /tmp/t_cp.o && echo COMPILE_OK; cd ..
```
Expected: `COMPILE_OK` (no undefined `CORE_PIN20_*` — proves the guards and the 20-pin block resolve).

- [ ] **Step 4: Verify the existing boards still compile (guard regression)**

Run (from `teensy4/`):
```bash
cd teensy4 && for d in ARDUINO_TEENSY40 ARDUINO_TEENSY41 ARDUINO_TEENSY_MICROMOD; do \
  printf '#include "core_pins.h"\n' > /tmp/t_cp.cpp; \
  $GXX -c $CPU -D__IMXRT1062__ -D$d -DF_CPU=600000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH -DARDUINO=10813 -DTEENSYDUINO=159 $CXX_ONLY -I. /tmp/t_cp.cpp -o /tmp/t_cp.o && echo "OK $d"; done; cd ..
```
Expected: `OK ARDUINO_TEENSY40`, `OK ARDUINO_TEENSY41`, `OK ARDUINO_TEENSY_MICROMOD`.

- [ ] **Step 5: Commit**

```bash
git add teensy4/core_pins.h
git commit -m "core_pins: add EVKB 20-pin map + sub-20-pin fast-path guards"
```

---

### Task 6: digital.c — sub-20-pin guard

**Files:**
- Modify: `teensy4/digital.c:54-93` (info table)

- [ ] **Step 1: Guard table entries for pins 20–39**

In `teensy4/digital.c`, the `digital_pin_to_info_PGM[]` array lists entries `CORE_PIN0…CORE_PIN39` unconditionally, then `#if CORE_NUM_DIGITAL > 40`. Wrap the pin 20–39 entries:
1. Insert `#if CORE_NUM_DIGITAL > 20` before the `{&CORE_PIN20_PORTREG, ...}` line.
2. Insert `#endif` after the `{&CORE_PIN39_PORTREG, ...}` line (directly before the existing `#if CORE_NUM_DIGITAL > 40`).

Result:
```c
	{&CORE_PIN19_PORTREG, &CORE_PIN19_CONFIG, &CORE_PIN19_PADCONFIG, CORE_PIN19_BITMASK},
#if CORE_NUM_DIGITAL > 20
	{&CORE_PIN20_PORTREG, &CORE_PIN20_CONFIG, &CORE_PIN20_PADCONFIG, CORE_PIN20_BITMASK},
	/* ... pins 21..38 unchanged ... */
	{&CORE_PIN39_PORTREG, &CORE_PIN39_CONFIG, &CORE_PIN39_PADCONFIG, CORE_PIN39_BITMASK},
#endif
#if CORE_NUM_DIGITAL > 40
```

- [ ] **Step 2: Verify digital.c compiles for the EVKB**

Run (from `teensy4/`):
```bash
cd teensy4 && $GCC -c $CPU $DEFS -I. digital.c -o /tmp/digital.o && echo COMPILE_OK; cd ..
```
Expected: `COMPILE_OK`.

- [ ] **Step 3: Commit**

```bash
git add teensy4/digital.c
git commit -m "digital: guard pin 20-39 info-table entries for sub-20-pin boards"
```

---

### Task 7: analog.c — EVKB ADC channel table

**Files:**
- Modify: `teensy4/analog.c:41-86` (`pin_to_channel[]`)

- [ ] **Step 1: Add an EVKB pin_to_channel table**

In `teensy4/analog.c`, wrap the existing `pin_to_channel[]` body so the EVKB gets its own 20-entry, pin-indexed table. Change the array to:
```c
const uint8_t pin_to_channel[] = { // pg 482
#if defined(ARDUINO_MIMXRT1060_EVKB)
	12,	// 0  D0  AD_B1_07
	11,	// 1  D1  AD_B1_06
	255,	// 2  D2  AD_B0_11 (not ADC)
	13,	// 3  D3  AD_B1_08
	255,	// 4  D4  AD_B0_09 (not ADC)
	255,	// 5  D5  AD_B0_10 (not ADC)
	7,	// 6  D6  AD_B1_02
	8,	// 7  D7  AD_B1_03
	255,	// 8  D8  AD_B0_03 (not ADC)
	255,	// 9  D9  AD_B0_02 (not ADC)
	255,	// 10 D10 SD_B0_01 (not ADC)
	255,	// 11 D11 SD_B0_02 (not ADC)
	255,	// 12 D12 SD_B0_03 (not ADC)
	255,	// 13 D13 SD_B0_00 (not ADC)
	15,	// 14 A0  AD_B1_10
	0,	// 15 A1  AD_B1_11
	9,	// 16 A2  AD_B1_04
	10,	// 17 A3  AD_B1_05
	6,	// 18 A4  AD_B1_01
	5,	// 19 A5  AD_B1_00
#else
	7,	// 0/A0  AD_B1_02
	/* ... existing Teensy entries unchanged ... */
#endif
};
```
(Insert `#if defined(ARDUINO_MIMXRT1060_EVKB) … #else` after the `{ // pg 482` line, and `#endif` before the closing `};`. Leave the original Teensy entries and the inner `#ifdef ARDUINO_TEENSY41` block intact inside the `#else`.)

- [ ] **Step 2: Verify analog.c compiles for the EVKB**

Run (from `teensy4/`):
```bash
cd teensy4 && $GCC -c $CPU $DEFS -I. analog.c -o /tmp/analog.o && echo COMPILE_OK; cd ..
```
Expected: `COMPILE_OK`.

- [ ] **Step 3: Commit**

```bash
git add teensy4/analog.c
git commit -m "analog: add EVKB ADC channel map (A0-A5 + analog-capable digitals)"
```

---

### Task 8: pwm.c — EVKB PWM pin table

**Files:**
- Modify: `teensy4/pwm.c:49-114` (`pwm_pin_info[]`)

- [ ] **Step 1: Add an EVKB pwm_pin_info table**

In `teensy4/pwm.c`, wrap the existing `pwm_pin_info[]` body and add the EVKB block. Change the array opening to:
```c
const struct pwm_pin_info_struct pwm_pin_info[] = {
#if defined(ARDUINO_MIMXRT1060_EVKB)
	{0, M(1, 0), 0, 0},  // 0  D0  AD_B1_07 - no PWM
	{0, M(1, 0), 0, 0},  // 1  D1  AD_B1_06 - no PWM
	{0, M(1, 0), 0, 0},  // 2  D2  AD_B0_11 - no PWM
	{1, M(4, 0), 1, 1},  // 3  D3  AD_B1_08 - FlexPWM4_0_A
	{0, M(1, 0), 0, 0},  // 4  D4  AD_B0_09 - no PWM (LED)
	{0, M(1, 0), 0, 0},  // 5  D5  AD_B0_10 - no PWM (submodule unverified)
	{2, M(3, 2), 0, 1},  // 6  D6  AD_B1_02 - QuadTimer3_2
	{2, M(3, 3), 0, 1},  // 7  D7  AD_B1_03 - QuadTimer3_3
	{1, M(1, 1), 0, 4},  // 8  D8  AD_B0_03 - FlexPWM1_1_X
	{1, M(1, 0), 0, 4},  // 9  D9  AD_B0_02 - FlexPWM1_0_X
	{1, M(1, 0), 2, 1},  // 10 D10 SD_B0_01 - FlexPWM1_0_B
	{1, M(1, 1), 1, 1},  // 11 D11 SD_B0_02 - FlexPWM1_1_A
	{1, M(1, 1), 2, 1},  // 12 D12 SD_B0_03 - FlexPWM1_1_B
	{1, M(1, 0), 1, 1},  // 13 D13 SD_B0_00 - FlexPWM1_0_A
	{0, M(1, 0), 0, 0},  // 14 A0  AD_B1_10 - no PWM
	{0, M(1, 0), 0, 0},  // 15 A1  AD_B1_11 - no PWM
	{0, M(1, 0), 0, 0},  // 16 A2  AD_B1_04 - no PWM
	{0, M(1, 0), 0, 0},  // 17 A3  AD_B1_05 - no PWM
	{2, M(3, 1), 0, 1},  // 18 A4  AD_B1_01 - QuadTimer3_1
	{2, M(3, 0), 0, 1},  // 19 A5  AD_B1_00 - QuadTimer3_0
#else
	{1, M(1, 1), 0, 4},  // FlexPWM1_1_X   0  // AD_B0_03
	/* ... existing Teensy entries unchanged ... */
#endif
};
```
(Insert `#if defined(ARDUINO_MIMXRT1060_EVKB) … #else` after the array's opening `{`, and `#endif` before the closing `};`. Keep all existing Teensy entries and their inner `#ifdef` blocks inside the `#else`.)

- [ ] **Step 2: Verify pwm.c compiles for the EVKB**

Run (from `teensy4/`):
```bash
cd teensy4 && $GCC -c $CPU $DEFS -I. pwm.c -o /tmp/pwm.o && echo COMPILE_OK; cd ..
```
Expected: `COMPILE_OK`.

- [ ] **Step 3: Commit**

```bash
git add teensy4/pwm.c
git commit -m "pwm: add EVKB PWM pin table (best-effort submodules)"
```

---

### Task 9: Chip-level guards (treat EVKB as Teensy 4.1)

**Files (each modified at the listed line):**
- `teensy4/HardwareSerial.cpp:69`, `:734`
- `teensy4/HardwareSerial.h:132`, `:342`, `:386`
- `teensy4/HardwareSerial1.cpp:56`
- `teensy4/HardwareSerial5.cpp:55`
- `teensy4/HardwareSerial8.cpp:33`
- `teensy4/avr/eeprom.h:36`
- `teensy4/eeprom.c:44`
- `teensy4/extmem.c:38`
- `teensy4/smalloc.h:49`
- `teensy4/startup.c:75`, `:186`, `:353`
- `teensy4/usb_desc.c:120`

- [ ] **Step 1: Add the EVKB define to each Teensy-4.1 guard**

For every site above, extend the Teensy-4.1 condition to also match the EVKB. Apply mechanically:
- `#if defined(ARDUINO_TEENSY41)` → `#if defined(ARDUINO_TEENSY41) || defined(ARDUINO_MIMXRT1060_EVKB)`
- `#ifdef ARDUINO_TEENSY41` → `#if defined(ARDUINO_TEENSY41) || defined(ARDUINO_MIMXRT1060_EVKB)`
- `#elif defined(ARDUINO_TEENSY41)` → `#elif defined(ARDUINO_TEENSY41) || defined(ARDUINO_MIMXRT1060_EVKB)`
- `#if defined(__IMXRT1062__) && defined(ARDUINO_TEENSY41)` → `#if defined(__IMXRT1062__) && (defined(ARDUINO_TEENSY41) || defined(ARDUINO_MIMXRT1060_EVKB))`

(The matching `#endif // ARDUINO_TEENSY41` comment at `startup.c:561` needs no change.) Re-find any stragglers with:
```bash
grep -rn "ARDUINO_TEENSY41" teensy4/ | grep -vE "core_pins.h|pins_arduino.h|digital.c|analog.c|pwm.c|bootdata.c"
```

- [ ] **Step 2: Verify each changed translation unit compiles for the EVKB**

Run (from `teensy4/`):
```bash
cd teensy4 && \
for f in HardwareSerial HardwareSerial1 HardwareSerial5 HardwareSerial8 usb_desc; do \
  case $f in usb_desc) $GCC -c $CPU $DEFS -I. $f.c -o /tmp/o.o;; \
  *) $GXX -c $CPU $DEFS $CXX_ONLY -I. $f.cpp -o /tmp/o.o;; esac && echo "OK $f"; done && \
for f in eeprom extmem startup; do $GCC -c $CPU $DEFS -I. $f.c -o /tmp/o.o && echo "OK $f"; done; cd ..
```
Expected: `OK` for each (HardwareSerial, HardwareSerial1, HardwareSerial5, HardwareSerial8, usb_desc, eeprom, extmem, startup).

- [ ] **Step 3: Commit**

```bash
git add teensy4/HardwareSerial*.cpp teensy4/HardwareSerial.h teensy4/avr/eeprom.h teensy4/eeprom.c teensy4/extmem.c teensy4/smalloc.h teensy4/startup.c teensy4/usb_desc.c
git commit -m "core: treat MIMXRT1060-EVKB as Teensy 4.1 for chip-level config"
```

---

### Task 10: Makefile EVKB option block

**Files:**
- Modify: `teensy4/Makefile:30-37` (the MCU option comments)

- [ ] **Step 1: Add a commented EVKB option block**

In `teensy4/Makefile`, after the Teensy 4.1 commented option block, add:
```makefile
# Use these lines for NXP MIMXRT1060-EVKB
#MCU = IMXRT1062
#MCU_LD = imxrt1060_evkb.ld
#MCU_DEF = ARDUINO_MIMXRT1060_EVKB
```

- [ ] **Step 2: Verify the Makefile still parses**

Run:
```bash
make -n -C teensy4 ARDUINOPATH=/Applications/Teensyduino.app/Contents/Java main.elf >/dev/null && echo MAKEFILE_OK
```
Expected: `MAKEFILE_OK`.

- [ ] **Step 3: Commit**

```bash
git add teensy4/Makefile
git commit -m "Makefile: add commented MIMXRT1060-EVKB build option"
```

---

### Task 11: Full integration build + regression

**Files:** none (verification only)

- [ ] **Step 1: Build the full core for the EVKB**

Run:
```bash
extras/mimxrt1060_evkb/build_check.sh ARDUINO_MIMXRT1060_EVKB imxrt1060_evkb.ld
```
Expected: `BUILD OK: ARDUINO_MIMXRT1060_EVKB / imxrt1060_evkb.ld`.

- [ ] **Step 2: Regression-build all existing boards**

Run:
```bash
extras/mimxrt1060_evkb/build_check.sh ARDUINO_TEENSY40 imxrt1062.ld
extras/mimxrt1060_evkb/build_check.sh ARDUINO_TEENSY41 imxrt1062_t41.ld
extras/mimxrt1060_evkb/build_check.sh ARDUINO_TEENSY_MICROMOD imxrt1062_mm.ld
```
Expected: `BUILD OK` for all three.

- [ ] **Step 3: Assert the generated map values**

Run (from `teensy4/`):
```bash
cd teensy4 && printf '#include <Arduino.h>\nint a[]={LED_BUILTIN,NUM_DIGITAL_PINS,NUM_ANALOG_INPUTS,CORE_NUM_TOTAL_PINS};\n' > /tmp/t_map.cpp \
  && $GXX -E $DEFS $CXX_ONLY $CPU -I. /tmp/t_map.cpp | grep -E "int a\[\]" ; cd ..
```
Expected: `int a[]={(4),20,6,20};` (LED=D4, 20 digital, 6 analog, 20 total).

- [ ] **Step 4: Commit (no code change; this task is verification)**

No commit needed — proceed to Task 12 once all builds pass.

---

### Task 12: Add the board to Teensyduino + record it in repo

**Files:**
- Modify (install, not repo): `/Applications/Teensyduino.app/Contents/Java/hardware/teensy/avr/boards.txt`
- Create: `extras/mimxrt1060_evkb/README.md`

- [ ] **Step 1: Write the in-repo README with the exact boards.txt stanza**

Create `extras/mimxrt1060_evkb/README.md`:

````markdown
# NXP MIMXRT1060-EVKB — Teensy core board

Adds the MIMXRT1060-EVKB (i.MX RT1062) as a board reusing the `teensy4` core.

## Scope / status
- Appears in Arduino IDE and **compiles cleanly**; accurate 20-pin Arduino map
  (D0–D15, A0–A5), `LED_BUILTIN` = D4 (GPIO_AD_B0_09).
- **Not** hardware-verified. Peripheral (UART/SPI/I2C/PWM) instance/mux is
  best-effort ("pin numbers only"); SDRAM/USB bring-up deferred.

## Install the board listing
The Teensy `boards.txt` lives at:
`/Applications/Teensyduino.app/Contents/Java/hardware/teensy/avr/boards.txt`
This file is overwritten by Teensyduino updates — re-append the stanza below
after updating. Also copy the modified `teensy4` core files (or symlink the
install's `cores/teensy4` to this repo's `teensy4`) so the IDE builds the new
board code.

```
mimxrt1060evkb.name=NXP MIMXRT1060-EVKB
mimxrt1060evkb.build.board=MIMXRT1060_EVKB
mimxrt1060evkb.build.flags.ld=-Wl,--gc-sections,--relax "-T{build.core.path}/imxrt1060_evkb.ld"
mimxrt1060evkb.upload.tool=teensyloader
mimxrt1060evkb.upload.protocol=teensy
mimxrt1060evkb.build.core=teensy4
mimxrt1060evkb.build.mcu=imxrt1062
mimxrt1060evkb.build.warn_data_percentage=99
mimxrt1060evkb.build.toolchain=arm/bin/
mimxrt1060evkb.build.command.gcc=arm-none-eabi-gcc
mimxrt1060evkb.build.command.g++=arm-none-eabi-g++
mimxrt1060evkb.build.command.ar=arm-none-eabi-gcc-ar
mimxrt1060evkb.build.command.objcopy=arm-none-eabi-objcopy
mimxrt1060evkb.build.command.objdump=arm-none-eabi-objdump
mimxrt1060evkb.build.command.linker=arm-none-eabi-gcc
mimxrt1060evkb.build.command.size=arm-none-eabi-size
mimxrt1060evkb.build.flags.common=-g -Wall -ffunction-sections -fdata-sections -nostdlib
mimxrt1060evkb.build.flags.dep=-MMD
mimxrt1060evkb.build.flags.optimize=-Os
mimxrt1060evkb.build.flags.cpu=-mthumb -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16
mimxrt1060evkb.build.flags.defs=-D__IMXRT1062__ -DTEENSYDUINO=159
mimxrt1060evkb.build.flags.cpp=-std=gnu++14 -fno-exceptions -fpermissive -fno-rtti -fno-threadsafe-statics -felide-constructors -Wno-error=narrowing
mimxrt1060evkb.build.flags.c=
mimxrt1060evkb.build.flags.S=-x assembler-with-cpp
mimxrt1060evkb.build.flags.libs=-larm_cortexM7lfsp_math -lm -lstdc++
mimxrt1060evkb.menu.usb.serial=Serial
mimxrt1060evkb.menu.usb.serial.build.usbtype=USB_SERIAL
mimxrt1060evkb.menu.speed.600=600 MHz
mimxrt1060evkb.menu.speed.600.build.fcpu=600000000
mimxrt1060evkb.menu.speed.150=150 MHz
mimxrt1060evkb.menu.speed.150.build.fcpu=150000000
mimxrt1060evkb.menu.opt.o2std=Faster
mimxrt1060evkb.menu.opt.o2std.build.flags.optimize=-O2
mimxrt1060evkb.menu.opt.o2std.build.flags.ldspecs=
mimxrt1060evkb.menu.keys.en-us=US English
mimxrt1060evkb.menu.keys.en-us.build.keylayout=US_ENGLISH
```
(More USB/speed/opt/keys options can be copied verbatim from the `teensy41.*`
stanza if desired.)

## Flashing (user-owned; no HalfKay)
The EVKB has no Teensy bootloader. Flash the built `.hex`/`.bin` via the
on-board OpenSDA debug probe — e.g. drag-and-drop onto the DAPLink mass-storage
drive, or `pyocd`/`OpenOCD` over CMSIS-DAP. Default boot is QSPI NOR
(SW4[3:0] = OFFOFFONOFF).

## Build check (no IDE/hardware needed)
`extras/mimxrt1060_evkb/build_check.sh ARDUINO_MIMXRT1060_EVKB imxrt1060_evkb.ld`
````

- [ ] **Step 2: Append the stanza to the installed boards.txt**

Append the same stanza (lines from `mimxrt1060evkb.name=` through
`mimxrt1060evkb.menu.keys.en-us.build.keylayout=US_ENGLISH`) to the end of
`/Applications/Teensyduino.app/Contents/Java/hardware/teensy/avr/boards.txt`.
Edit that file directly (the user authorized this).

- [ ] **Step 3: Verify the stanza is present and well-formed**

Run:
```bash
grep -c "^mimxrt1060evkb\." /Applications/Teensyduino.app/Contents/Java/hardware/teensy/avr/boards.txt
```
Expected: a count ≥ 30 (the stanza lines are present).

- [ ] **Step 4: Commit the repo-side README**

```bash
git add extras/mimxrt1060_evkb/README.md
git commit -m "docs: record EVKB boards.txt stanza and flashing notes"
```

---

## Self-Review

**Spec coverage:**
- Board identity / build wiring → Task 12 (boards.txt) + Task 1/10 (Makefile, harness).
- Linker (8MB flash, SDRAM note) → Task 2.
- FlexSPI boot config (IS25WP064A 8MB) → Task 3.
- Accurate pin map (20 pins) → Task 5 (core_pins), Task 4 (pins_arduino), Task 7 (analog), Task 8 (pwm).
- Sub-20-pin guards → Task 5 (core_pins switches) + Task 6 (digital.c table).
- Chip-level guards → Task 9.
- Verification (compile gate + regression + map asserts) → Tasks 1, 5, 11.
- boards.txt direct edit + extras record → Task 12.
All spec sections map to a task.

**Placeholder scan:** No TBD/TODO; every code change shows complete code or an exact mechanical rule with a grep to locate sites and a compile gate to confirm completeness.

**Type/name consistency:** `ARDUINO_MIMXRT1060_EVKB`, `MIMXRT1060_EVKB` (build.board), `imxrt1060_evkb.ld`, `mimxrt1060evkb.*`, `build_check.sh`, `pin_to_channel`, `pwm_pin_info`, `digital_pin_to_info_PGM`, `CORE_NUM_DIGITAL > 20`, `LED_BUILTIN=4` are used consistently across tasks. Pin counts (20 digital / 6 analog) and the `> 20` guard threshold are consistent in Tasks 4, 5, 6, 11.
