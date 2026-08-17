/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2019 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "HardwareSerial.h"
#include "core_pins.h"

#define IRQ_PRIORITY 64  // 0 = highest priority, 255 = lowest

void IRQHandler_Serial2();
static uint8_t tx_buffer2[64];
static uint8_t rx_buffer2[64];

extern volatile uint32_t iomuxc_no_daisy;

// Serial2 = LPUART2 = the M.2 socket (J54) Bluetooth HCI UART.
//   TXD  GPIO_DISP_B2_10 ALT2  -> J54 pin 32 (card UART_RXD)
//   RXD  GPIO_DISP_B2_11 ALT2  <- J54 pin 22 (card UART_TXD)
//
// Two board facts worth knowing before using this port:
//   * RX IS DEAD AS BUILT.  R1901 (module->MCU) is DNP on the EVKB RevC3, so
//     the card's TX reaches the level shifter and stops there.  Transmit works;
//     nothing is ever received.
//   * No flow control is offered.  CTS/RTS would mux GPIO_DISP_B2_12/13, which
//     on this board are RGMII1_PHY_INTB and ETHPHY_RST_B -- asserting RTS holds
//     the gigabit PHY in reset.
// Both are documented in docs/m2-evkb-revc3.md in the rt1176-evkb repo.
const HardwareSerialIMXRT::hardware_t UART2_Hardware = {
	1, IRQ_LPUART2, &IRQHandler_Serial2, &serialEvent2,
	CCM_LPCG87_DIRECT,                        // lpcg_register  (0x40CC6AE0)
	CCM_CLOCK_ROOT26_CONTROL,                 // clock_root_reg (0x40CC0D00)
	(0u /*mux OscRC48MDiv2*/ | 0u /*div=1*/), // clock_root_val -> 24 MHz
	IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_10, 2u, IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_10, // TXD ALT2
	IOMUXC_SW_MUX_CTL_PAD_GPIO_DISP_B2_11, 2u, IOMUXC_SW_PAD_CTL_PAD_GPIO_DISP_B2_11, // RXD ALT2
	iomuxc_no_daisy, 0u,                      // LPUART2 has no RXD daisy register
	IRQ_PRIORITY,
};

HardwareSerialIMXRT Serial2(0x40080000, &UART2_Hardware, tx_buffer2, sizeof(tx_buffer2),
                            rx_buffer2, sizeof(rx_buffer2));

void IRQHandler_Serial2() { Serial2.IRQHandler(); }

void serialEvent2() __attribute__((weak));
void serialEvent2() {}
