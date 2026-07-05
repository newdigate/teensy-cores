#include "wm8962.h"
WM8962Codec Codec;

bool WM8962Codec::writeReg(uint16_t reg, uint16_t val) {
    bus->beginTransmission(addr);
    bus->write((uint8_t)0x00);            // subaddress hi (reg <= 0xFF)
    bus->write((uint8_t)reg);             // subaddress lo
    bus->write((uint8_t)(val >> 8));      // data hi (16-bit, big-endian)
    bus->write((uint8_t)(val & 0xFF));    // data lo
    return bus->endTransmission() == 0;
}
bool WM8962Codec::readReg(uint16_t reg, uint16_t *val) {
    bus->beginTransmission(addr);
    bus->write((uint8_t)0x00); bus->write((uint8_t)reg);
    if (bus->endTransmission(false) != 0) return false;   // repeated start
    if (bus->requestFrom(addr, (uint8_t)2) != 2) return false;
    uint8_t hi = bus->read(), lo = bus->read();
    *val = ((uint16_t)hi << 8) | lo;
    return true;
}
bool WM8962Codec::pollSeqDone() {
    for (int i = 0; i < 100000; i++) {
        uint16_t s; if (!readReg(0x5D, &s)) return false;
        if ((s & 0x1) == 0) return true;
    }
    return false;
}
// (reg,val) init subset transcribed from fsl_wm8962.c WM8962_Init for
// I2S/16-bit/48kHz/MCLK=24.576MHz/slave/DAC->headphone.
static const uint16_t INIT[][2] = {
    {0x0F,0x6243},{0x81,0x0000},{0x9B,0x0000},{0x08,0x09E4},{0x19,0x01FE},{0x1A,0x01E0},
};
static const uint16_t SEQ[] = {0x80, 0x92, 0xE8};       // DACToHP, AnaIn, SpkWake
static const uint16_t POST[][2] = {
    {0x07,0x0002},{0x0A,0x01C0},{0x0B,0x01C0},{0x02,0x016B},{0x03,0x016B},
    {0x1B,0x0010},{0x38,0x0006},
};
bool WM8962Codec::begin(TwoWire &b, uint8_t a) {
    bus = &b; addr = a;
    for (auto &e : INIT) if (!writeReg(e[0], e[1])) return false;
    for (uint16_t id : SEQ) {
        if (!writeReg(0x57, 0x0020)) return false;
        if (!writeReg(0x5A, id))     return false;
        if (!pollSeqDone())          return false;
    }
    for (auto &e : POST) if (!writeReg(e[0], e[1])) return false;
    return true;
}
