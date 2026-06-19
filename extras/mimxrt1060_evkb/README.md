# NXP MIMXRT1060-EVKB — Teensy core board

Adds the MIMXRT1060-EVKB (i.MX RT1062) as a board reusing the `teensy4` core.

## Scope / status
- Appears in Arduino IDE and **compiles cleanly**; accurate 20-pin Arduino map
  (D0–D15, A0–A5), `LED_BUILTIN` = D4 (GPIO_AD_B0_09).
- **Not** hardware-verified. Peripheral (UART/SPI/I2C/PWM) instance/mux is
  best-effort ("pin numbers only"); SDRAM/USB bring-up deferred.

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
