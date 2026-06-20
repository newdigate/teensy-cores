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
