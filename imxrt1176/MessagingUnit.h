/* MessagingUnit.h - CM7-side driver for the i.MX RT1176 MU (Messaging Unit).
 *
 * The MU is a symmetric 4-channel mailbox + 4-channel doorbell + 3-bit flag
 * link between the CM7 (Processor A, MUA @ 0x40C48000) and the CM4 (Processor
 * B, MUB).  This driver operates the A side.  It is a clean-room API written
 * against RM ch.35 whose shape mirrors the NXP SDK classic `mu` driver
 * (fsl_mu.h v2.3.3) -- the flavor RT1176 actually builds -- so behavior is
 * cross-checked, not copied.  Register layout, bit positions and the
 * GIP-W1C / GIR-auto-clear semantics were HW-verified on the EVKB by
 * evkb/dualcore_mu_test (2026-07-16).
 *
 * Channels are 0..3.  In every 4-bit MU field n=0 is the HIGH-order bit; the
 * MU_SR_n / MU_CR_n macros in imxrt1176.h encode that, so callers use 0..3.
 *
 * This header is in the public domain (author: Nicholas Newdigate).
 */
#ifndef MessagingUnit_h
#define MessagingUnit_h

#include <stdint.h>
#include "imxrt1176.h"

#ifdef __cplusplus

class MessagingUnit {
public:
    static const uint8_t CHANNELS = 4;

    /* Enable the MU NVIC interrupt (IRQ 118) and clear all callbacks.  Safe to
     * call before or after the CM4 is booted; the mailbox works polled without
     * begin(), but the onReceive/onDoorbell callbacks require it. */
    void begin();
    void end();

    /* ---- Mailbox: 32-bit data messages, 4 channels ---- */
    /* Non-blocking send: writes TRn only if it is empty (TEn set).  Returns
     * false (message NOT sent) if the previous message on this channel has not
     * yet been read by the CM4. */
    bool trySend(uint8_t ch, uint32_t msg);
    /* Blocking send: spins until TRn is empty, then writes.  Mirrors
     * MU_SendMsg -- use only when the CM4 is guaranteed to be draining. */
    void send(uint8_t ch, uint32_t msg);
    /* True when RRn holds an unread message from the CM4 (RFn set). */
    bool available(uint8_t ch);
    /* Non-blocking receive: if RRn is full, stores it in *out and returns true
     * (reading RRn frees the CM4's TEn); otherwise returns false. */
    bool tryReceive(uint8_t ch, uint32_t *out);
    /* Blocking receive: spins until RRn is full, then returns it. */
    uint32_t receive(uint8_t ch);

    /* ---- Doorbell: general-purpose interrupt, 4 channels ---- */
    /* Request the CM4's GIPn (rising edge on our GIRn).  Returns false if a
     * previous request on this channel is still pending (GIRn not yet cleared
     * by the CM4's ack) -- matches MU_TriggerInterrupts' kStatus_Fail guard. */
    bool trigger(uint8_t ch);
    /* True while our GIRn is still set (the CM4 has not yet acked). */
    bool triggerPending(uint8_t ch);
    /* True when the CM4 has rung our doorbell (our GIPn pending). */
    bool doorbell(uint8_t ch);
    /* Acknowledge our GIPn (write-1-to-clear).  This also HW-clears the CM4's
     * GIRn, completing the doorbell handshake. */
    void acknowledge(uint8_t ch);

    /* ---- General-purpose flags: 3-bit Fn, reflected in the peer's SR ---- */
    void writeFlags(uint8_t flags3);   /* our CR.Fn -> CM4's SR.Fn */
    uint8_t readFlags();               /* CM4's CR.Fn reflected in our SR.Fn */

    /* Raw A-side status register, with the always-set silicon bit 9 masked
     * off so flag comparisons behave as documented. */
    uint32_t status();

    /* ---- Interrupt-driven receive / doorbell (needs begin()) ---- */
    /* cb(ch, msg) fires from the MU ISR when RRn fills; the ISR reads RRn
     * (freeing the CM4) before calling cb. */
    void onReceive(uint8_t ch, void (*cb)(uint8_t ch, uint32_t msg));
    /* cb(ch) fires from the MU ISR when the CM4 rings GIPn; the ISR
     * acknowledges GIPn after cb returns (de-asserting the interrupt). */
    void onDoorbell(uint8_t ch, void (*cb)(uint8_t ch));
    void enableReceiveInterrupt(uint8_t ch, bool enable);
    void enableDoorbellInterrupt(uint8_t ch, bool enable);

    /* Called by the shared IRQ_MU vector -- not part of the public API. */
    void isr();

private:
    void (* volatile rx_cb_[CHANNELS])(uint8_t, uint32_t);
    void (* volatile db_cb_[CHANNELS])(uint8_t);
};

extern MessagingUnit MU;

#endif /* __cplusplus */
#endif /* MessagingUnit_h */
