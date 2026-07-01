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
#include "Arduino.h"

#define UART_CLOCK 24000000

#define CTRL_ENABLE 		(LPUART_CTRL_TE | LPUART_CTRL_RE | LPUART_CTRL_RIE | LPUART_CTRL_ILIE)
#define CTRL_TX_ACTIVE		(CTRL_ENABLE | LPUART_CTRL_TIE)
#define CTRL_TX_COMPLETING	(CTRL_ENABLE | LPUART_CTRL_TCIE)
#define CTRL_TX_INACTIVE	CTRL_ENABLE

// Task 6: clock root + LPCG gate + pin mux + pad config all live here.  Kept an
// empty stub for now so begin()'s baud/FIFO/CTRL programming compiles and links.
void HardwareSerialIMXRT::configure_hardware(void) {
    // 1. LPUART1 clock root = OscRC48MDiv2 (24 MHz), divider 1  (clock_root_val = mux0|div0)
    hardware->clock_root_reg = hardware->clock_root_val;
    // 2. enable the LPUART1 LPCG clock gate (CCM->LPCG[86].DIRECT = ON)
    hardware->lpcg_register = 0x1u;
    // 3. pad mux (ALT0 = LPUART1 TXD/RXD) + pad control.
    //    PAD_CFG 0x02: fast slew, high drive strength, weak pull-down (exact value from the
    //    EVKB SDK pin_mux.c for GPIO_AD_24/25 LPUART1 console pads).
    const uint32_t PAD_CFG = 0x02u;
    hardware->tx_mux_reg = hardware->tx_mux_val;  hardware->tx_pad_reg = PAD_CFG;
    hardware->rx_mux_reg = hardware->rx_mux_val;  hardware->rx_pad_reg = PAD_CFG;
    // 4. route the RX pad into LPUART1_RXD (input daisy = GPIO_AD_25)
    hardware->rx_select_input_reg = hardware->rx_select_input_val;
}

void HardwareSerialIMXRT::begin(uint32_t baud, uint16_t format)
{
	configure_hardware();

	IMXRT_LPUART_t *port = (IMXRT_LPUART_t *)port_addr;
	float base = (float)UART_CLOCK / (float)baud;
	float besterr = 1e20;
	int bestdiv = 1;
	int bestosr = 4;
	for (int osr=4; osr <= 32; osr++) {
		float div = base / (float)osr;
		int divint = (int)(div + 0.5f);
		if (divint < 1) divint = 1;
		else if (divint > 8191) divint = 8191;
		float err = ((float)divint - div) / div;
		if (err < 0.0f) err = -err;
		if (err <= besterr) {
			besterr = err;
			bestdiv = divint;
			bestosr = osr;
		}
	}
	rx_buffer_head_ = 0;
	rx_buffer_tail_ = 0;
	tx_buffer_head_ = 0;
	tx_buffer_tail_ = 0;

	transmitting_ = 0;

	port->BAUD = LPUART_BAUD_OSR(bestosr - 1) | LPUART_BAUD_SBR(bestdiv)
		| (bestosr <= 8 ? LPUART_BAUD_BOTHEDGE : 0);
	port->PINCFG = 0;

	// Enable the transmitter, receiver and enable receiver interrupt
	attachInterruptVector(hardware->irq, hardware->irq_handler);
	NVIC_SET_PRIORITY(hardware->irq, hardware->irq_priority);
	NVIC_ENABLE_IRQ(hardware->irq);

	// FIFO size / watermarks -- both TX and RX FIFO fixed at 4 (register value 1).
	// Keep the FIFO enabled + watermarks so the RX ISR's (WATER>>24)&7 count works.
	uint8_t tx_water = 2;
	uint8_t rx_water = 2;
	port->WATER = LPUART_WATER_RXWATER(rx_water) | LPUART_WATER_TXWATER(tx_water);
	port->FIFO |= LPUART_FIFO_TXFE | LPUART_FIFO_RXFE;

	// Enable the transmitter, receiver, and RX interrupt.
	port->CTRL = LPUART_CTRL_TE | LPUART_CTRL_RE | LPUART_CTRL_RIE;
};

void HardwareSerialIMXRT::end(void)
{
	IMXRT_LPUART_t *port = (IMXRT_LPUART_t *)port_addr;
	while (transmitting_) yield();  // wait for buffered data to send
	port->CTRL = 0;	// disable the TX and RX ...

	rx_buffer_head_ = 0;
	rx_buffer_tail_ = 0;
}

int HardwareSerialIMXRT::availableForWrite(void)
{
	uint32_t head, tail;

	head = tx_buffer_head_;
	tail = tx_buffer_tail_;
	if (head >= tail) return tx_buffer_total_size_ - 1 - head + tail;
	return tail - head - 1;
}

int HardwareSerialIMXRT::available(void)
{
	IMXRT_LPUART_t *port = (IMXRT_LPUART_t *)port_addr;
	uint32_t head, tail;

	// WATER> 0 so IDLE involved may want to check if port has already has RX data to retrieve
	__disable_irq();
	head = rx_buffer_head_;
	tail = rx_buffer_tail_;
	int avail;
	if (head >= tail) avail = head - tail;
	else avail = rx_buffer_total_size_ + head - tail;
	avail += (port->WATER >> 24) & 0x7;
	__enable_irq();
	return avail;
}

int HardwareSerialIMXRT::peek(void)
{
	IMXRT_LPUART_t *port = (IMXRT_LPUART_t *)port_addr;
	uint32_t head, tail;

	head = rx_buffer_head_;
	tail = rx_buffer_tail_;
	if (head == tail) {
		__disable_irq();
		head = rx_buffer_head_;  // reread head to make sure no ISR happened
		if (head == tail) {
			// Still empty Now check for stuff in FIFO Queue.
			int c = -1;	// assume nothing to return
			if (port->WATER & 0x7000000) {
				c = port->DATA & 0x3ff;		// Use only up to 10 bits of data
				// But we don't want to throw it away...
				// since queue is empty, just going to reset to front of queue...
				rx_buffer_head_ = 1;
				rx_buffer_tail_ = 0;
				rx_buffer_[1] = c;
			}
			__enable_irq();
			return c;
		}
		__enable_irq();

	}
	if (++tail >= rx_buffer_total_size_) tail = 0;
	if (tail < rx_buffer_size_) {
		return rx_buffer_[tail];
	} else {
		return rx_buffer_storage_[tail-rx_buffer_size_];
	}
}

int HardwareSerialIMXRT::read(void)
{
	IMXRT_LPUART_t *port = (IMXRT_LPUART_t *)port_addr;
	uint32_t head, tail;
	int c;

	head = rx_buffer_head_;
	tail = rx_buffer_tail_;
	if (head == tail) {
		__disable_irq();
		head = rx_buffer_head_;  // reread head to make sure no ISR happened
		if (head == tail) {
			// Still empty Now check for stuff in FIFO Queue.
			c = -1;	// assume nothing to return
			if (port->WATER & 0x7000000) {
				c = port->DATA & 0x3ff;		// Use only up to 10 bits of data
			}
			__enable_irq();
			return c;
		}
		__enable_irq();

	}
	if (++tail >= rx_buffer_total_size_) tail = 0;
	if (tail < rx_buffer_size_) {
		c = rx_buffer_[tail];
	} else {
		c = rx_buffer_storage_[tail-rx_buffer_size_];
	}
	rx_buffer_tail_ = tail;
	return c;
}

void HardwareSerialIMXRT::flush(void)
{
	while (transmitting_) yield(); // wait
}

size_t HardwareSerialIMXRT::write(uint8_t c)
{
	IMXRT_LPUART_t *port = (IMXRT_LPUART_t *)port_addr;
	uint32_t head = tx_buffer_head_;
	if (++head >= tx_buffer_total_size_) head = 0;
	while (tx_buffer_tail_ == head) {           // buffer full: wait for ISR drain
		if ((port->STAT & LPUART_STAT_TDRE) && ((port->CTRL & LPUART_CTRL_TIE) == 0)) break;
		yield();
	}
	if (head < tx_buffer_size_) tx_buffer_[head] = c; else tx_buffer_storage_[head - tx_buffer_size_] = c;
	transmitting_ = 1;
	tx_buffer_head_ = head;
	port->CTRL |= (LPUART_CTRL_TIE | LPUART_CTRL_TCIE);
	return 1;
}

void HardwareSerialIMXRT::IRQHandler()
{
	IMXRT_LPUART_t *port = (IMXRT_LPUART_t *)port_addr;
	uint32_t head, tail, n;
	uint32_t ctrl;

	// See if we have stuff to read in.
	if (port->STAT & (LPUART_STAT_RDRF | LPUART_STAT_IDLE)) {
		// See how many bytes are pending.
		uint8_t avail = (port->WATER >> 24) & 0x7;
		if (avail) {
			uint32_t newhead;
			head = rx_buffer_head_;
			tail = rx_buffer_tail_;
			do {
				n = port->DATA & 0x3ff;		// Use only up to 10 bits of data
				newhead = head + 1;

				if (newhead >= rx_buffer_total_size_) newhead = 0;
				if (newhead != rx_buffer_tail_) {
					head = newhead;
					if (newhead < rx_buffer_size_) {
						rx_buffer_[head] = n;
					} else {
						rx_buffer_storage_[head-rx_buffer_size_] = n;
					}
				}
			} while (--avail > 0) ;
			rx_buffer_head_ = head;
		}

		// If it was an idle status clear the idle
		if (port->STAT & LPUART_STAT_IDLE) {
			port->STAT |= LPUART_STAT_IDLE;	// writing a 1 to idle should clear it.
		}
	}

	// See if we are transmitting and room in buffer.
	ctrl = port->CTRL;
	if ((ctrl & LPUART_CTRL_TIE) && (port->STAT & LPUART_STAT_TDRE))
	{
		head = tx_buffer_head_;
		tail = tx_buffer_tail_;
		do {
			if (head == tail) break;
			if (++tail >= tx_buffer_total_size_) tail = 0;
			if (tail < tx_buffer_size_) {
				n = tx_buffer_[tail];
			} else {
				n = tx_buffer_storage_[tail-tx_buffer_size_];
			}
			port->DATA = n;
		} while (((port->WATER >> 8) & 0x7) < 4); 	// need to compute properly
		tx_buffer_tail_ = tail;
		if (head == tail) {
			port->CTRL &= ~LPUART_CTRL_TIE;
			port->CTRL |= LPUART_CTRL_TCIE; // Actually wondering if we can just leave this one on...
		}
	}

	if ((ctrl & LPUART_CTRL_TCIE) && (port->STAT & LPUART_STAT_TC))
	{
		transmitting_ = 0;
		port->CTRL &= ~LPUART_CTRL_TCIE;
	}
}
