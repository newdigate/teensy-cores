/* USB CDC serial object instantiation for the MIMXRT1176 core (SerialUSB Phase 2).
 * Serial is the USB CDC virtual serial port; SerialUSB is a reference alias to it.
 *
 * Arduino.h (not usb_serial.h directly) is included first, matching the teensy4
 * usb_inst.cpp pattern: usb_serial.h's inline usb_serial_class methods call
 * yield()/millis() (declared by core_pins.h, pulled in transitively via
 * Arduino.h) without declaring them itself. */
#include <Arduino.h>
#include "usb_serial.h"

#if defined(CDC_STATUS_INTERFACE) && defined(CDC_DATA_INTERFACE)
usb_serial_class Serial;
usb_serial_class &SerialUSB = Serial;
#endif

// Keyboard is provided by usb_keyboard.h (pulled in via Arduino.h), backed by
// the C functions in usb_keyboard.c.
#if defined(KEYBOARD_INTERFACE)
usb_keyboard_class Keyboard;
#endif

#if defined(MOUSE_INTERFACE)
usb_mouse_class Mouse;
#endif
