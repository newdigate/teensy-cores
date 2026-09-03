// Pure, dependency-free register logic for LPUART receiver RTS (RXRTSE) hardware
// flow control.  No Arduino/CMSIS includes, so it host-compiles and is unit
// tested (imxrt1176/extras/lpuart_rts_test/).  MIT.
#pragma once
#include <stdint.h>

namespace lpuart_rts {
    // The MODIR value for RECEIVER flow control on an i.MX RT1170 LPUART:
    //   bit3   RXRTSE  = 1  (receiver deasserts RTS as the RX FIFO nears full)
    //   bit1   TXRTSE  = 0  (RM 71.5.1.11: never set both RXRTSE and TXRTSE)
    //   bits9:8 RTSWATER = watermark (0..3; the RX FIFO is 4 deep)
    // This is the whole register decision, isolated for testing.
    inline uint32_t modir(uint8_t rtswater) {
        return (1u << 3) | (((uint32_t)(rtswater & 0x3u)) << 8);
    }
}
