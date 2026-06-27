# NXP MIMXRT1060-EVKB — Teensy core board

Adds the MIMXRT1060-EVKB (i.MX RT1062) as a board reusing the `teensy4` core.

## Scope / status
- Appears in Arduino IDE and **compiles cleanly**; accurate Arduino map of 23
  core pins: D0–D13 = pins 0–13, A0–A5 = pins 14–19, plus three non-header pads:
    - pin 20 = `GPIO_AD_B0_08` — the on-board **user LED** (`LED_BUILTIN`,
      **active low**: `digitalWrite(LED_BUILTIN, LOW)` turns it on),
    - pin 21 = `GPIO_AD_B0_12` — **LPUART1_TX**, pin 22 = `GPIO_AD_B0_13` —
      **LPUART1_RX**: `Serial6` is wired to these so it reaches the OpenSDA
      virtual COM port (the EVKB's built-in USB serial).
- **Hardware-verified:** boots from QSPI and blinks the user LED on a real
  MIMXRT1060-EVKB. Other peripheral instance/mux (SPI/I2C/PWM, other UARTs)
  remains best-effort; SDRAM/USB-device bring-up deferred.
- Note: `LED_BUILTIN` is `GPIO_AD_B0_08` per the NXP SDK board files
  (`BOARD_USER_LED_GPIO_PIN 8`), **not** `_09` (which is Arduino header D4).

## Gotchas (read these — they cost real debugging time)

1. **The flash is ISSI, not Winbond.** The boot flash is an `IS25WP064AJBLE`,
   so `bootdata.c` carries an EVKB-specific FlexSPI config (8 dummy cycles + a
   `0xC0` read-register command, 120 MHz, generated from the MCUXpresso SDK).
   The stock Teensy FlexSPI config is tuned for Teensy's Winbond part (6 dummy
   cycles) and the boot ROM **will not read the IS25 with it** — the image
   programs but never runs (dark LED, no crash). Don't revert that block.

2. **User LED = `GPIO_AD_B0_08`, active-low.** Not `_09`. Use `LED_BUILTIN`
   (pin 20); `digitalWrite(LED_BUILTIN, LOW)` turns it **on**.

3. **Arduino's bundled `Blink` hard-codes `int led = 13;`** — it ignores
   `LED_BUILTIN`, so it toggles D13/SCK (no LED) and looks dead. Use
   `LED_BUILTIN` (or pin 20) in your sketch.

4. **Serial to the on-board USB COM port is `Serial6`, not `Serial`.** The
   EVKB's OpenSDA virtual COM is LPUART1 = `Serial6` (pins 21/22 here). Teensy's
   `Serial` is USB-device CDC (separate connector, not the OpenSDA port).

5. **Only use pins/peripherals that exist in the 23-pin map.** A `Serial`/`SPI`/
   `Wire` object whose *default* Teensy pins fall outside 0–22 will index past
   the pin table in `begin()` and write to a bad register (we hit `Serial6`'s
   stock pins 24/25 → a fault near address `0x62`, caught by the MPU).

## Peripheral notes — SPI, I2C (Wire), SD (SdFat)

The EVKB is a densely-populated EVK: almost every pad has a dedicated function,
so unlike a Teensy there is **no free, header-dedicated SPI bus**.

### SPI shares pins with the microSD slot — by board design
The only pads the EVKB labels as SPI are `GPIO_SD_B0_02`/`_03`
(`SD1_D0/J24[4]/SPI_MOSI` and `SD1_D1/J24[5]/SPI_MISO`). So the default `SPI`
(LPSPI1 on `SD_B0_00–03`, core pins 10–13) **is** the board's SPI — and it is
the **same J24 pins as the microSD card**. You cannot use the default `SPI` and
the microSD at the same time. This is the EVKB-correct mapping, not a porting
bug, so the SPI library is left on LPSPI1/SD_B0. Every other LPSPI pad set also
steals an on-board peripheral, so there is no conflict-free default SPI:

| LPSPI pads | steals |
|---|---|
| LPSPI1 / SD_B0 (default `SPI`) | microSD card (J24) |
| LPSPI1 / EMC_27–30 | SDRAM (SEMC) |
| LPSPI3 / AD_B1_12–15 (`SPI1`) | camera CSI + audio SAI1 (J35) |
| LPSPI4 / B0_00–03 (`SPI2`) | parallel LCD (LCDIF) |
| LPSPI3 / AD_B0_00–03 | **USB OTG1** (`USB_OTG1_PWR/ID/OC`, J24) — do **not** use: breaks USB |

To run SPI + microSD together you must move the default `SPI` onto one of the
"optional peripheral" buses above (e.g. LPSPI3/AD_B1_12–15, sacrificing the
camera) — not done by default.

### Wire (I2C)
`Wire` = LPI2C1 on pins 18/19 (`AD_B1_01/00`) — the Arduino I2C header, also the
on-board accelerometer/codec/camera bus; works as-is. `Wire1` (LPI2C3) and
`Wire2` (LPI2C4) are remapped for the EVKB to pins 1/0 and 22/21 (alternate-
function pads; `Wire2` shares the `Serial6` pins).

### SD card / SdFat
The built-in SDIO path needs **no SdFat change**: the `SdioTeensy` driver
already targets **uSDHC1 on `SD_B0_00–05`**, exactly the EVKB microSD wiring.
One EVKB-only addition: the microSD is powered by a load switch on
`GPIO_AD_B1_03` (**core pin 7**), which the NXP SDK enables by driving the pad
**LOW**; the Teensy boards have no such switch, so the EVKB build of SdFat drives
pin 7 in `SdioCard::begin()`. **Pin 7 is therefore reserved for SD power when
using SdFat.** (Verified end-to-end in QEMU except the power switch, which QEMU
does not model — confirm the polarity on real hardware.)

## Install into the platform the IDE actually compiles with

> ⚠️ **Install location matters.** The Arduino IDE / `arduino-builder` loads
> `teensy:avr` from **every** hardware folder on its path. If you have Teensy
> installed *both* as a standalone `Teensyduino.app` bundle *and* via the Boards
> Manager (`~/Library/Arduino15/packages/teensy/hardware/avr/<version>/`), the
> Boards Manager copy **overrides** the bundle. Editing the bundle's `boards.txt`
> then has no effect on compilation — you get `Error resolving FQBN: board ...
> not found`. Add the board to whichever platform actually wins.
>
> Find the active one by compiling any Teensy sketch with verbose output and
> noting the `teensy:avr@<version>` path in the error/log, or just edit the
> Boards Manager copy if you have one.

Both the **board listing** and the **modified core** must land in that platform's
`hardware/teensy/avr/<ver>/`:

1. **Core code** — apply this repo's `teensy4/` EVKB changes to
   `<platform>/cores/teensy4/` (add `imxrt1060_evkb.ld`, and the EVKB `#elif`/
   `#if CORE_NUM_DIGITAL > 20` additions to `core_pins.h`, `digital.c`,
   `analog.c`, `pwm.c`, `pins_arduino.h`, `bootdata.c`, plus the EVKB define on
   the Teensy-4.1 guards in `HardwareSerial*`, `eeprom.c`, `avr/eeprom.h`,
   `extmem.c`, `smalloc.h`, `startup.c`, `usb_desc.c`). The EVKB blocks are
   self-contained and apply cleanly to any recent Teensyduino core.

2. **Linker** — `cp <platform>/cores/teensy4/imxrt1062_t41.ld
   <platform>/cores/teensy4/imxrt1060_evkb.ld` (EVKB flash is 8 MB, same as 4.1).

3. **Board stanza** — derive it from that platform's own `teensy41` block so it
   inherits the **correct compiler flags (notably `-std=gnu++17`)** and all
   USB/speed/opt/keys menus, then override three keys:

   ```bash
   BT=<platform>/boards.txt
   grep '^teensy41\.' "$BT" | sed 's/^teensy41\./mimxrt1060evkb./' \
     | sed -e 's|^mimxrt1060evkb.name=.*|mimxrt1060evkb.name=NXP MIMXRT1060-EVKB|' \
           -e 's|^mimxrt1060evkb.build.board=.*|mimxrt1060evkb.build.board=MIMXRT1060_EVKB|' \
           -e 's|^mimxrt1060evkb.build.flags.ld=.*|mimxrt1060evkb.build.flags.ld=-Wl,--gc-sections,--relax "-T{build.core.path}/imxrt1060_evkb.ld"|' \
     >> "$BT"
   ```

> Do **not** hand-write `-std=gnu++14`: the modern `teensy4` core uses C++17
> (`if constexpr`, `std::is_arithmetic_v` in `IntervalTimer.h`), so a `gnu++14`
> stanza fails to compile. Deriving from `teensy41` avoids this.

These files are overwritten by Teensyduino/Boards-Manager updates — re-apply
after updating. (Verified: this exact setup compiles the stock `Blink` example
for the EVKB via the real `arduino-builder`.)

## Flashing (user-owned; no HalfKay)
The EVKB has no Teensy bootloader. Flash the built `.hex`/`.bin` via the
on-board OpenSDA debug probe — e.g. drag-and-drop onto the DAPLink mass-storage
drive, or `pyocd`/`OpenOCD` over CMSIS-DAP. Default boot is QSPI NOR
(SW4[3:0] = OFFOFFONOFF).

## Build check (no IDE/hardware needed)
`extras/mimxrt1060_evkb/build_check.sh ARDUINO_MIMXRT1060_EVKB imxrt1060_evkb.ld`
