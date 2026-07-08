// This header file is in the public domain.

#ifndef Arduino_h
#define Arduino_h

#include "WProgram.h"
#include "pins_arduino.h"
// Expose the USB CDC `Serial` object through Arduino.h, matching stock
// Teensyduino.  WProgram.h has already defined Stream (usb_serial_class's base)
// by this point, and usb_serial.h is include-guarded, so gates that include it
// explicitly are unaffected.  Without this, any library that references the
// standard Arduino `Serial` (e.g. SdFat's FatFile/FatVolume ls()/printName()
// under ENABLE_ARDUINO_SERIAL) fails to compile with "'Serial' not declared".
#include "usb_serial.h"

#endif
