#include "I2S.h"
bool I2SClass::begin(uint32_t sampleRate) { (void)sampleRate; return false; }
void I2SClass::end() {}
void I2SClass::write(const int16_t *s, size_t n) { (void)s; (void)n; }
