/* USB CDC-ACM + HID keyboard descriptors for the MIMXRT1176 core.
 * Composite CDC + Keyboard, byte layout ported from cores/teensy4/usb_desc.{c,h}. */
#pragma once
#include <stdint.h>

#define VENDOR_ID            0x1209   /* pid.codes generic (placeholder, not PJRC) */
#define PRODUCT_ID           0x0004   /* bumped per composite-shape change (+joystick 0x0004): forces a macOS descriptor re-read */
#define EP0_SIZE             64
#define NUM_ENDPOINTS        7        /* CDC 4 + keyboard EP5 + mouse EP6 + joystick EP7 */
#define NUM_INTERFACE        5        /* CDC(0,1) + keyboard(2) + mouse(3) + joystick(4) */
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

/* Keyboard HID (boot protocol, 8-byte report, no REPORT_ID) */
#define KEYBOARD_INTERFACE       2
#define KEYBOARD_ENDPOINT        5        /* interrupt IN (0x85) */
#define KEYBOARD_SIZE            8
#define KEYBOARD_INTERVAL        1
#define KEYBOARD_HID_DESC_OFFSET 84       /* 75 (end of CDC block) + 9 (kbd iface desc) */
#ifndef LAYOUT_US_ENGLISH                 /* build macros usually pass -DLAYOUT_US_ENGLISH; guard avoids a redefinition warning and keeps the core self-sufficient if they don't */
#define LAYOUT_US_ENGLISH
#endif

/* Mouse HID (relative report-ID 1 + absolute report-ID 2; NON-boot interface) */
#define MOUSE_INTERFACE       3
#define MOUSE_ENDPOINT        6        /* interrupt IN (0x86) */
#define MOUSE_SIZE            8
#define MOUSE_INTERVAL        2
#define MOUSE_HID_DESC_OFFSET 109      /* 100 (end of keyboard block) + 9 (mouse iface desc) */

/* Joystick HID (12-byte report: 32 buttons + hat + 6 axes; no REPORT_ID) */
#define JOYSTICK_INTERFACE       4
#define JOYSTICK_ENDPOINT        7        /* interrupt IN (0x87) */
#define JOYSTICK_SIZE            12
#define JOYSTICK_INTERVAL        1
#define JOYSTICK_HID_DESC_OFFSET 134      /* 125 (end of mouse block) + 9 (joystick iface desc) */

#define CONFIG_DESC_SIZE     150     /* 75 CDC + 25 keyboard + 25 mouse + 25 joystick */

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
#define ENDPOINT5_CONFIG  (ENDPOINT_RECEIVE_UNUSED | ENDPOINT_TRANSMIT_INTERRUPT) /* 0x00CC0002 */
#define ENDPOINT6_CONFIG  (ENDPOINT_RECEIVE_UNUSED | ENDPOINT_TRANSMIT_INTERRUPT) /* 0x00CC0002 */
#define ENDPOINT7_CONFIG  (ENDPOINT_RECEIVE_UNUSED | ENDPOINT_TRANSMIT_INTERRUPT) /* 0x00CC0002 */

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
