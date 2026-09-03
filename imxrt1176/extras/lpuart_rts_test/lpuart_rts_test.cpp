#include "lpuart_rts.h"
#include <stdio.h>
static int g_fails = 0, g_checks = 0;
#define CHECK(c) do { g_checks++; if (!(c)) { g_fails++; printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); } } while (0)

int main() {
    // RXRTSE (bit3) set for every watermark; TXRTSE (bit1) never set; RTSWATER carried.
    for (uint8_t w = 0; w <= 3; w++) {
        uint32_t m = lpuart_rts::modir(w);
        CHECK(m & (1u << 3));               // RXRTSE set
        CHECK(!(m & (1u << 1)));            // TXRTSE clear (RM: never both)
        CHECK(((m >> 8) & 0x3u) == w);      // RTSWATER field == w
    }
    // No stray bits outside RXRTSE | RTSWATER.
    CHECK(lpuart_rts::modir(0) == (1u << 3));
    CHECK(lpuart_rts::modir(3) == ((1u << 3) | (3u << 8)));
    printf("lpuart_rts_test: %d checks, %d failures\n", g_checks, g_fails);
    return g_fails ? 1 : 0;
}
