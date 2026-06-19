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
