/* USB device controller core for the MIMXRT1176 (SerialUSB Phase 1).
 * Ported from cores/teensy4/usb_dev.h.  Iso/SOF/timer APIs dropped.
 *
 * Original copyright (c) 2019 PJRC.COM, LLC. (MIT-style, see teensy4 sources)
 */

#pragma once
#include "imxrt1176.h"

typedef struct transfer_struct transfer_t;
struct transfer_struct {
	uint32_t next;
	volatile uint32_t status;
	uint32_t pointer0;
	uint32_t pointer1;
	uint32_t pointer2;
	uint32_t pointer3;
	uint32_t pointer4;
	uint32_t callback_param;
};

#ifdef __cplusplus
extern "C" {
#endif

void usb_init(void);

void usb_config_rx(uint32_t ep, uint32_t packet_size, int do_zlp, void (*cb)(transfer_t *));
void usb_config_tx(uint32_t ep, uint32_t packet_size, int do_zlp, void (*cb)(transfer_t *));

void usb_prepare_transfer(transfer_t *transfer, const void *data, uint32_t len, uint32_t param);
void usb_transmit(int endpoint_number, transfer_t *transfer);
void usb_receive(int endpoint_number, transfer_t *transfer);
uint32_t usb_transfer_status(const transfer_t *transfer);

extern void (*usb_timer0_callback)(void);

extern volatile uint8_t usb_configuration;

/* CDC line-state globals (names/types match teensy4 usb_serial).
 * Defined in usb.c for Phase 1 until usb_serial.c arrives in Phase 2. */
extern uint32_t usb_cdc_line_coding[2];
extern volatile uint32_t usb_cdc_line_rtsdtr_millis;
extern volatile uint8_t usb_cdc_line_rtsdtr;

#ifdef __cplusplus
}
#endif
