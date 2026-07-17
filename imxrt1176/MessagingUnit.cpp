/* MessagingUnit.cpp - CM7-side i.MX RT1176 MU driver.  See MessagingUnit.h.
 *
 * Clean-room against RM ch.35; behavior cross-checked with the NXP classic
 * `mu` driver (fsl_mu.h v2.3.3) and HW-verified by evkb/dualcore_mu_test
 * (2026-07-16).  Public domain (author: Nicholas Newdigate).
 */
#include "MessagingUnit.h"
#include "core_pins.h"    // attachInterruptVector, NVIC_ENABLE_IRQ, IRQ_MU

/* All four GIRn request bits, CR[19:16].  fsl_mu masks these off on every
 * non-trigger CR write so reconfiguring interrupts/flags cannot glitch a
 * doorbell; we follow that convention (caveat: do not reconfigure while a
 * trigger() is outstanding on the same channel). */
#define MU_CR_GIR_ALL 0x000F0000u
/* All interrupt-enable + request bits, CR[31:16] (GIE|RIE|TIE|GIR). */
#define MU_CR_IE_ALL  0xFFFF0000u

MessagingUnit MU;

static void mu_isr_trampoline(void)
{
    MU.isr();
}

void MessagingUnit::begin()
{
    for (uint8_t n = 0; n < CHANNELS; n++) {
        rx_cb_[n] = 0;
        db_cb_[n] = 0;
    }
    attachInterruptVector(IRQ_MU, &mu_isr_trampoline);
    NVIC_ENABLE_IRQ(IRQ_MU);
}

void MessagingUnit::end()
{
    NVIC_DISABLE_IRQ(IRQ_MU);
    MUA_CR &= ~MU_CR_IE_ALL;   /* drop all enables + any pending request */
    for (uint8_t n = 0; n < CHANNELS; n++) {
        rx_cb_[n] = 0;
        db_cb_[n] = 0;
    }
}

/* ---- Mailbox ---- */
bool MessagingUnit::trySend(uint8_t ch, uint32_t msg)
{
    if (ch >= CHANNELS || !(MUA_SR & MU_SR_TE(ch))) {
        return false;
    }
    MUA_TR(ch) = msg;
    return true;
}

void MessagingUnit::send(uint8_t ch, uint32_t msg)
{
    if (ch >= CHANNELS) {
        return;
    }
    while (!(MUA_SR & MU_SR_TE(ch))) {
    }
    MUA_TR(ch) = msg;
}

bool MessagingUnit::available(uint8_t ch)
{
    return ch < CHANNELS && (MUA_SR & MU_SR_RF(ch)) != 0;
}

bool MessagingUnit::tryReceive(uint8_t ch, uint32_t *out)
{
    if (ch >= CHANNELS || !(MUA_SR & MU_SR_RF(ch))) {
        return false;
    }
    *out = MUA_RR(ch);   /* reading RRn frees the CM4's TEn */
    return true;
}

uint32_t MessagingUnit::receive(uint8_t ch)
{
    if (ch >= CHANNELS) {
        return 0;
    }
    while (!(MUA_SR & MU_SR_RF(ch))) {
    }
    return MUA_RR(ch);
}

/* ---- Doorbell ---- */
bool MessagingUnit::trigger(uint8_t ch)
{
    if (ch >= CHANNELS) {
        return false;
    }
    uint32_t cr = MUA_CR;
    if (cr & MU_CR_GIR(ch)) {
        return false;   /* previous request not yet acked by the CM4 */
    }
    MUA_CR = cr | MU_CR_GIR(ch);
    return true;
}

bool MessagingUnit::triggerPending(uint8_t ch)
{
    return ch < CHANNELS && (MUA_CR & MU_CR_GIR(ch)) != 0;
}

bool MessagingUnit::doorbell(uint8_t ch)
{
    return ch < CHANNELS && (MUA_SR & MU_SR_GIP(ch)) != 0;
}

void MessagingUnit::acknowledge(uint8_t ch)
{
    if (ch < CHANNELS) {
        MUA_SR = MU_SR_GIP(ch);   /* W1C: also HW-clears the CM4's GIRn */
    }
}

/* ---- Flags ---- */
void MessagingUnit::writeFlags(uint8_t flags3)
{
    uint32_t cr = MUA_CR & ~MU_CR_GIR_ALL;
    MUA_CR = (cr & ~MU_CR_Fn_MASK) | (flags3 & MU_CR_Fn_MASK);
}

uint8_t MessagingUnit::readFlags()
{
    return (uint8_t)(MUA_SR & MU_SR_Fn_MASK);
}

uint32_t MessagingUnit::status()
{
    return MUA_SR & ~MU_SR_BIT9;   /* mask the always-set silicon bit 9 */
}

/* ---- Interrupt-driven ---- */
void MessagingUnit::enableReceiveInterrupt(uint8_t ch, bool enable)
{
    if (ch >= CHANNELS) {
        return;
    }
    uint32_t cr = MUA_CR & ~MU_CR_GIR_ALL;
    if (enable) {
        cr |= MU_CR_RIE(ch);
    } else {
        cr &= ~MU_CR_RIE(ch);
    }
    MUA_CR = cr;
}

void MessagingUnit::enableDoorbellInterrupt(uint8_t ch, bool enable)
{
    if (ch >= CHANNELS) {
        return;
    }
    uint32_t cr = MUA_CR & ~MU_CR_GIR_ALL;
    if (enable) {
        cr |= MU_CR_GIE(ch);
    } else {
        cr &= ~MU_CR_GIE(ch);
    }
    MUA_CR = cr;
}

void MessagingUnit::onReceive(uint8_t ch, void (*cb)(uint8_t, uint32_t))
{
    if (ch >= CHANNELS) {
        return;
    }
    rx_cb_[ch] = cb;
    enableReceiveInterrupt(ch, cb != 0);
}

void MessagingUnit::onDoorbell(uint8_t ch, void (*cb)(uint8_t))
{
    if (ch >= CHANNELS) {
        return;
    }
    db_cb_[ch] = cb;
    enableDoorbellInterrupt(ch, cb != 0);
}

void MessagingUnit::isr()
{
    uint32_t sr = MUA_SR;
    uint32_t cr = MUA_CR;

    /* Receive: RFn full and RIEn enabled.  Read RRn (clears RFn, de-asserting
     * the interrupt) before invoking the callback. */
    for (uint8_t n = 0; n < CHANNELS; n++) {
        if ((sr & MU_SR_RF(n)) && (cr & MU_CR_RIE(n))) {
            uint32_t v = MUA_RR(n);
            if (rx_cb_[n]) {
                rx_cb_[n](n, v);
            }
        }
    }
    /* Doorbell: GIPn pending and GIEn enabled.  Acknowledge (W1C) after the
     * callback to de-assert the interrupt and clear the CM4's GIRn. */
    for (uint8_t n = 0; n < CHANNELS; n++) {
        if ((sr & MU_SR_GIP(n)) && (cr & MU_CR_GIE(n))) {
            if (db_cb_[n]) {
                db_cb_[n](n);
            }
            MUA_SR = MU_SR_GIP(n);
        }
    }
}
