/* USB CDC-ACM descriptors for the MIMXRT1176 core (SerialUSB Phase 1).
 * CDC-only subset ported from cores/teensy4/usb_desc.{c,h}. */
#pragma once
#include <stdint.h>

#define VENDOR_ID            0x1209   /* pid.codes generic (placeholder, not PJRC) */
#define PRODUCT_ID           0x0001
#define EP0_SIZE             64
#define NUM_ENDPOINTS        4
#define NUM_INTERFACE        2
#define CDC_STATUS_INTERFACE 0
#define CDC_DATA_INTERFACE   1
#define CDC_ACM_ENDPOINT     2        /* interrupt IN (0x82) */
#define CDC_RX_ENDPOINT      3        /* bulk OUT (0x03)     */
#define CDC_TX_ENDPOINT      4        /* bulk IN  (0x84)     */
#define CDC_ACM_SIZE         16
#define CDC_RX_SIZE_480      512
#define CDC_TX_SIZE_480      512
#define CDC_RX_SIZE_12       64
#define CDC_TX_SIZE_12       64
#define CONFIG_DESC_SIZE     75

/* Values written to USB1_ENDPTCTRLn in SET_CONFIGURATION (from teensy4 usb_desc.h). */
#define ENDPOINT_TRANSMIT_UNUSED    0x00020000
#define ENDPOINT_TRANSMIT_BULK      0x00C80000
#define ENDPOINT_TRANSMIT_INTERRUPT 0x00CC0000
#define ENDPOINT_RECEIVE_UNUSED     0x00000002
#define ENDPOINT_RECEIVE_BULK       0x000000C8
#define ENDPOINT_RECEIVE_INTERRUPT  0x000000CC
#define ENDPOINT2_CONFIG  (ENDPOINT_RECEIVE_UNUSED | ENDPOINT_TRANSMIT_INTERRUPT) /* 0x00CC0002 */
#define ENDPOINT3_CONFIG  (ENDPOINT_RECEIVE_BULK   | ENDPOINT_TRANSMIT_UNUSED)    /* 0x000200C8 */
#define ENDPOINT4_CONFIG  (ENDPOINT_RECEIVE_UNUSED | ENDPOINT_TRANSMIT_BULK)      /* 0x00C80002 */

#define KEYCODE_TYPE uint8_t

#define LSB(n) ((n) & 255)
#define MSB(n) (((n) >> 8) & 255)

typedef struct {
    uint16_t wValue;
    uint16_t wIndex;
    const uint8_t *addr;
    uint16_t length;
} usb_descriptor_list_t;

struct usb_string_descriptor_struct {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t wString[];
};

extern const usb_descriptor_list_t usb_descriptor_list[];
