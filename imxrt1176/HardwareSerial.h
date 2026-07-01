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

#ifndef HardwareSerial_h
#define HardwareSerial_h

#include "imxrt1176.h"

// This is a reduced RT1176 port of the teensy4 HardwareSerial driver.  It keeps
// the abstract HardwareSerial base, the HardwareSerialIMXRT ring-buffer driver,
// and a single console instance (Serial1 on LPUART1).  RTS/CTS flow control,
// half-duplex, 9-bit mode, DMA, and XBAR triggering were dropped (YAGNI).

#define SERIAL_7E1 0x02
#define SERIAL_7O1 0x03
#define SERIAL_8N1 0x00
#define SERIAL_8E1 0x06
#define SERIAL_8O1 0x07
#define SERIAL_7E1_RXINV 0x12
#define SERIAL_7O1_RXINV 0x13
#define SERIAL_8N1_RXINV 0x10
#define SERIAL_8E1_RXINV 0x16
#define SERIAL_8O1_RXINV 0x17
#define SERIAL_7E1_TXINV 0x22
#define SERIAL_7O1_TXINV 0x23
#define SERIAL_8N1_TXINV 0x20
#define SERIAL_8E1_TXINV 0x26
#define SERIAL_8O1_TXINV 0x27
#define SERIAL_7E1_RXINV_TXINV 0x32
#define SERIAL_7O1_RXINV_TXINV 0x33
#define SERIAL_8N1_RXINV_TXINV 0x30
#define SERIAL_8E1_RXINV_TXINV 0x36
#define SERIAL_8O1_RXINV_TXINV 0x37
// We have 1/2 bit stop setting
#define SERIAL_2STOP_BITS 0x100
#define SERIAL_8E2 (SERIAL_8E1 | SERIAL_2STOP_BITS)
#define SERIAL_8O2 (SERIAL_8O1 | SERIAL_2STOP_BITS)
#define SERIAL_8E2_RXINV (SERIAL_8E1_RXINV | SERIAL_2STOP_BITS)
#define SERIAL_8O2_RXINV (SERIAL_8O1_RXINV | SERIAL_2STOP_BITS)
#define SERIAL_8E2_TXINV (SERIAL_8E1_TXINV | SERIAL_2STOP_BITS)
#define SERIAL_8O2_TXINV (SERIAL_8O1_TXINV | SERIAL_2STOP_BITS)
#define SERIAL_8E2_RXINV_TXINV (SERIAL_8E1_RXINV_TXINV | SERIAL_2STOP_BITS)
#define SERIAL_8O2_RXINV_TXINV (SERIAL_8O1_RXINV_TXINV | SERIAL_2STOP_BITS)
#define SERIAL_8N2 (SERIAL_8N1 | SERIAL_2STOP_BITS)
#define SERIAL_8N2_RXINV (SERIAL_8N1_RXINV | SERIAL_2STOP_BITS)
#define SERIAL_8N2_TXINV (SERIAL_8N1_TXINV | SERIAL_2STOP_BITS)
#define SERIAL_8N2_RXINV_TXINV (SERIAL_8N1_RXINV_TXINV | SERIAL_2STOP_BITS)

// bit0: parity, 0=even, 1=odd
// bit1: parity, 0=disable, 1=enable
// bit2: mode, 1=9bit, 0=8bit
// bit3: mode10: 1=10bit, 0=8bit
// bit4: rxinv, 0=normal, 1=inverted
// bit5: txinv, 0=normal, 1=inverted
// bit8: 2 stop bits

// Plain register-overlay struct for a single LPUART instance.  Field offsets
// match the LPUART register map exactly: VERID@0x00 .. WATER@0x2C.
typedef struct {
	volatile uint32_t VERID, PARAM, GLOBAL, PINCFG, BAUD, STAT, CTRL, DATA, MATCH, MODIR, FIFO, WATER;
} IMXRT_LPUART_t;

#ifdef __cplusplus
#include "Stream.h"
#include "core_pins.h"

#define BUFTYPE uint8_t

extern "C" {
	extern void IRQHandler_Serial1();
}

// HardwareSerial is now an abstract class, intended to allow FlexIO and USB Host
// serial devices to be compatible with libraries like MIDI, OSC, Adafruit_GPS
// which expect a pointer or reference to a HardwareSerial port.
class HardwareSerial : public Stream
{
public:
	virtual void begin(uint32_t baud, uint16_t format=0) = 0;
	virtual void end() = 0;
	virtual int available(void) = 0;
	virtual int peek(void) = 0;
	virtual int read(void) = 0;
	virtual void flush(void) = 0;
	virtual int availableForWrite(void) = 0;
	virtual size_t write(uint8_t) = 0;
	using Print::write;
	virtual operator bool() = 0;
};

class HardwareSerialIMXRT : public HardwareSerial
{
public:
	typedef struct {
		uint8_t serial_index;			// which object are we? 0 based
		IRQ_NUMBER_t irq;
		void (*irq_handler)(void);
		void (*_serialEvent)(void);
		volatile uint32_t &lpcg_register;	// CCM->LPCG[n].DIRECT
		volatile uint32_t &clock_root_reg;	// CCM->CLOCK_ROOT[n].CONTROL
		uint32_t clock_root_val;		// mux|div for 24 MHz
		volatile uint32_t &tx_mux_reg;	uint32_t tx_mux_val;	volatile uint32_t &tx_pad_reg;
		volatile uint32_t &rx_mux_reg;	uint32_t rx_mux_val;	volatile uint32_t &rx_pad_reg;
		volatile uint32_t &rx_select_input_reg;	uint32_t rx_select_input_val;
		uint16_t irq_priority;
	} hardware_t;
public:
	constexpr HardwareSerialIMXRT(uintptr_t myport, const hardware_t *myhardware,
		volatile BUFTYPE *_tx_buffer, size_t _tx_buffer_size,
		volatile BUFTYPE *_rx_buffer, size_t _rx_buffer_size) :
		port_addr(myport), hardware(myhardware),
		tx_buffer_(_tx_buffer), rx_buffer_(_rx_buffer), tx_buffer_size_(_tx_buffer_size),  rx_buffer_size_(_rx_buffer_size),
		tx_buffer_total_size_(_tx_buffer_size), rx_buffer_total_size_(_rx_buffer_size) {
	}
	// Initialize hardware serial port with baud rate and data format.  For a list
	// of all supported formats, see https://www.pjrc.com/teensy/td_uart.html
	void begin(uint32_t baud, uint16_t format=0);
	void end(void);
	// Returns the number of bytes which have been received and
	// can be fetched with read() or readBytes().
	virtual int available(void);
	// Returns the next received byte, but does not remove it from the receive
	// buffer.  Returns -1 if nothing has been received.
	virtual int peek(void);
	// Wait for all data written by print() or write() to actually transmit.
	virtual void flush(void);
	// Transmit a single byte
	virtual size_t write(uint8_t c);
	// Reads the next received byte, or returns -1 if nothing has been received.
	virtual int read(void);
	// Returns the number of bytes which may be transmitted by write() or print()
	// without waiting.
	virtual int availableForWrite(void);

	using Print::write;
	// Transmit a single byte
	size_t write(unsigned long n) { return write((uint8_t)n); }
	// Transmit a single byte
	size_t write(long n) { return write((uint8_t)n); }
	// Transmit a single byte
	size_t write(unsigned int n) { return write((uint8_t)n); }
	// Transmit a single byte
	size_t write(int n) { return write((uint8_t)n); }

	operator bool()			{ return true; }

private:
	const uintptr_t port_addr;
	const hardware_t * const hardware;

	volatile BUFTYPE 	*tx_buffer_;
	volatile BUFTYPE 	*rx_buffer_;
	volatile BUFTYPE	*rx_buffer_storage_ = nullptr;
	volatile BUFTYPE	*tx_buffer_storage_ = nullptr;
	size_t				tx_buffer_size_;
	size_t				rx_buffer_size_;
	size_t				tx_buffer_total_size_;
	size_t				rx_buffer_total_size_;
	volatile uint8_t 	transmitting_ = 0;
	volatile uint16_t 	tx_buffer_head_ = 0;
	volatile uint16_t 	tx_buffer_tail_ = 0;
	volatile uint16_t 	rx_buffer_head_ = 0;
	volatile uint16_t 	rx_buffer_tail_ = 0;

	void configure_hardware(void);

	void IRQHandler();
	friend void IRQHandler_Serial1();
};
// Serial1 hardware serial port (LPUART1 = EVKB VCOM console).  More detail at
// https://www.pjrc.com/teensy/td_uart.html
extern HardwareSerialIMXRT Serial1;
extern void serialEvent1(void);

#endif // __cplusplus

#endif
