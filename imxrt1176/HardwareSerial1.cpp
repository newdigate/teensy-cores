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

static void IRQHandler_Serial1();
static uint8_t tx_buffer1[64];
static uint8_t rx_buffer1[64];

const HardwareSerialIMXRT::hardware_t UART1_Hardware = {
	0, IRQ_LPUART1, &IRQHandler_Serial1, &serialEvent1,
	CCM_LPCG86_DIRECT,                        // lpcg_register  (0x40CC6AC0)
	CCM_CLOCK_ROOT25_CONTROL,                 // clock_root_reg (0x40CC0C80)
	(0u /*mux OscRC48MDiv2*/ | 0u /*div=1*/), // clock_root_val
	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_24, 0u, IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_24, // TX
	IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_25, 0u, IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_25, // RX
	IOMUXC_LPUART1_RXD_SELECT_INPUT, 0u,      // rx daisy -> GPIO_AD_25
	IRQ_PRIORITY,
};

HardwareSerialIMXRT Serial1(0x4007C000, &UART1_Hardware, tx_buffer1, sizeof(tx_buffer1),
                            rx_buffer1, sizeof(rx_buffer1));

static void IRQHandler_Serial1() { Serial1.IRQHandler(); }

void serialEvent1() __attribute__((weak));
void serialEvent1() {}
