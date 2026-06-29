# Arduino library support for the NXP MIMXRT1060-EVKB

Status of common Teensy/Arduino libraries on the **MIMXRT1060-EVKB** (i.MX RT1062),
built with `teensy:avr:mimxrt1060evkb`.

> **Prerequisite:** all of this assumes the **EVKB-aware `teensy4` core**
> (this `newdigate/teensy-cores` fork), which provides the EVKB pin tables,
> `CORE_INTn_PIN` (all 31 pins interrupt-capable), a `constexpr FS()`, and the
> EVKB SPI/Wire pin maps. Libraries below that "work natively" still rely on
> that core.
>
> EVKB SPI/peripheral mapping differs from Teensy 4.x: default `SPI` = **LPSPI1**
> (Teensy: LPSPI4), `SPI1` = LPSPI3, `SPI2` = LPSPI4. See `README.md` gotchas.

"QEMU" below = verified end-to-end on the i.MX RT1062 QEMU `mimxrt1060-evk`
machine (some of which required QEMU peripheral-model fixes); "compile" = builds
clean but not runtime-exercised in emulation (no model, or needs real hardware).

---

## Adjusted for the EVKB (code changes — use the `newdigate` fork)

| Library | Fork | What changed | Verified |
|---|---|---|---|
| **Wire** | [newdigate/Wire](https://github.com/newdigate/Wire) | `Wire1`→LPI2C3 (pins 1/0), `Wire2`→LPI2C4 (pins 22/21); `Wire`=LPI2C1 unchanged | QEMU (bus/codec) |
| **SdFat** | [newdigate/SdFat](https://github.com/newdigate/SdFat) | Drive the EVKB microSD power-enable (pin 7) in `SdioCard::begin()` | QEMU (uSDHC) |
| **SD** | [newdigate/SD](https://github.com/newdigate/SD) | Guard built-in-SD `DAT3` card-detect with `defined(_SD_DAT3)` | QEMU (uSDHC) |
| **Audio** | [newdigate/Audio](https://github.com/newdigate/Audio) | New `control_wm8960` codec driver (EVKB on-board WM8960 @ 0x1A); SAI1 I2S pad-mux (TX clock = master); GUI node | QEMU (SAI/WM8960) |
| **ST7735_t3** | [newdigate/ST7735_t3](https://github.com/newdigate/ST7735_t3) | Direct-LPSPI fast path uses LPSPI1 for default `SPI` (was hardcoded LPSPI4); ST7789/ST7796 inherit | QEMU (render) |
| **ILI9341_t3** | [newdigate/ILI9341_t3](https://github.com/newdigate/ILI9341_t3) | 53 hardcoded `IMXRT_LPSPI4_S` sites → per-board `ILI9341_LPSPI` macro (LPSPI1 on EVKB) | QEMU (render) |
| **ssd1351** | [newdigate/teensy_ssd1351](https://github.com/newdigate/teensy_ssd1351) | Same LPSPI-fast-path fix; fork re-based onto the Teensy-4.x-capable bundled source (upstream is T3.x-only) | compile |
| **LittleFS** | [newdigate/LittleFS](https://github.com/newdigate/LittleFS) | `LittleFS_Program` flash geometry for the EVKB (8 MB, `FLASH_SIZE 0x7C0000`, 64 KB sectors) | QEMU (format/RW) |
| **FlexCAN_T4** | [newdigate/FlexCAN_T4](https://github.com/newdigate/FlexCAN_T4) | `CAN3`/FLEXCAN3 mux on `AD_B0_14/15` (the EVKB TJA1057GT pads), not the Teensy EMC_36/37 (= EVKB SDRAM) | QEMU (loopback) |
| **FNET** | [newdigate/FNET](https://github.com/newdigate/FNET) | EVKB ENET: KSZ8081 PHY @ addr 2, reset on AD_B0_09, MDC/MDIO on EMC_40/41; RMII bring-up fixes — drive 50 MHz ref clock (`TX_CLK_DIR=1`), 50 MHz mode (PHY reg `0x1F` bit 7) set in the phy-init hook, advertise 10/100 (`ANAR=0x01E1`, board straps to 10M-only) | **HW (DHCP)** + QEMU |

---

## Work natively on the EVKB (no code changes)

Some carry a `newdigate` fork that only adds a short README/usage note.

| Library | Repo | Why it works as-is | Verified |
|---|---|---|---|
| **NativeEthernet** | [newdigate/NativeEthernet](https://github.com/newdigate/NativeEthernet) | Transport over FNET; all EVKB ENET specifics live in the FNET fork | **HW (DHCP)** + QEMU |
| **MIDI** | [newdigate/MIDI](https://github.com/newdigate/MIDI) | Transport-agnostic. Note: `MIDI_CREATE_DEFAULT_INSTANCE()` binds `Serial1`; use `Serial6` for the OpenSDA COM | QEMU (TX bytes) |
| **arm_math** (CMSIS-DSP) | [newdigate/arm_math](https://github.com/newdigate/arm_math) | CPU-only; selects target from `ARM_MATH_CMx`/FPU macros. Same Cortex-M7 lib as Teensy 4.x | QEMU (compute) |
| **USBHost_t36** | [newdigate/USBHost_t36](https://github.com/newdigate/USBHost_t36) | USB host = USB2/OTG2 on connector **J47**; host VBUS is a dedicated board rail (no software enable). Do **not** port the Teensy `EMC_40` VBUS code (= EVKB ENET/SDRAM pin) | compile (host mode not modelled) |
| **WDT_T4** | [newdigate/WDT_T4](https://github.com/newdigate/WDT_T4) | WDOG/RTWDOG/EWM are silicon-level; no board code | QEMU (fire/feed/reset for WDOG1, EWM, RTWDOG) |
| **EEPROM** | PaulStoffregen/EEPROM (upstream) | Uses the core's `eepromemu_flash_*` (EVKB region at 0x607C0000 in the core) | QEMU (RW) |
| **Encoder** | PaulStoffregen/Encoder (upstream) | Board-agnostic (core pin macros). Interrupt mode needs the core's `CORE_INTn_PIN` (in the cores fork); else auto-falls-back to polling | QEMU (ISR via core) |
| **Bounce2** | PaulStoffregen/Bounce2 (upstream) | Polling only — `digitalRead`/`pinMode`/`millis`, no peripheral access | compile |
| **SerialFlash** | [newdigate/SerialFlash](https://github.com/newdigate/SerialFlash) | The fork already carries the `begin(SPIClass&)` pointer fix; no EVKB-specific change | compile |
| **IntervalTimer** | core (Teensyduino) | PIT timer; board-agnostic | QEMU (PIT IRQ + rate) |
| **ILI9488_t3** | upstream | Derives its LPSPI base from the `SPI` object (`pa[0]`) → already board-correct | compile |
| **RA8875**, **XPT2046_Touchscreen** | upstream | Use the SPI *library* only (no direct-LPSPI fast path) → board-agnostic | compile |

---

## Notes
- **Display libraries** were the main bug class: those with a hardcoded
  `IMXRT_LPSPI4_S` fast path (ST7735_t3, ILI9341_t3, ssd1351) needed the EVKB
  LPSPI1 fix; those that derive the base from the `SPI` object (ILI9488_t3,
  ST7735_t3 after fix) or use only the SPI library (RA8875, XPT2046) were fine.
- **Not EVKB-portable in emulation:** USB *host* (no EHCI host model) and
  CAN-FD (`FlexCAN_T4FD` — FD not modelled). FlexIO has only a shallow model.
- Several QEMU peripheral-model fixes were made along the way to enable the
  runtime verification above (FlexSPI program, FlexCAN MAXMB, FEC checksums,
  NVIC byte reads, PIT clock frequency, watchdog warm-reset).
