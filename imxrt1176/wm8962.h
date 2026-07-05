#ifndef wm8962_h
#define wm8962_h
#include <stdint.h>
#include "Wire.h"
class WM8962Codec {
public:
    bool begin(TwoWire &bus, uint8_t addr = 0x1A);   // 48k/16-bit I2S slave, HP out
private:
    TwoWire *bus; uint8_t addr;
    bool writeReg(uint16_t reg, uint16_t val);
    bool readReg(uint16_t reg, uint16_t *val);
    bool pollSeqDone();     // read 0x5D until bit0==0
};
extern WM8962Codec Codec;
#endif
