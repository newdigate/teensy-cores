/* USB CDC-ACM descriptors for the MIMXRT1176 core (SerialUSB Phase 1).
 * Single-config CDC-ACM, byte layout ported from cores/teensy4/usb_desc.c. */
#include "usb_desc.h"

/* Device descriptor (18 bytes). */
static const uint8_t device_descriptor[] = {
    18, 1,                                  /* bLength, DEVICE            */
    0x00, 0x02,                             /* bcdUSB 2.00               */
    0xEF, 0x02, 0x01,                             /* class/sub/proto: IAD composite */
    EP0_SIZE,                               /* bMaxPacketSize0            */
    LSB(VENDOR_ID), MSB(VENDOR_ID),
    LSB(PRODUCT_ID), MSB(PRODUCT_ID),
    0x00, 0x01,                             /* bcdDevice 1.00            */
    1, 2, 0,                                /* iManufacturer, iProduct, iSerial=0 */
    1                                       /* bNumConfigurations        */
};

/* Keyboard HID report descriptor (boot protocol, 63 bytes; teensy4 usb_desc.c). */
#if defined(KEYBOARD_INTERFACE)
static uint8_t keyboard_report_desc[] = {
    0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x75, 0x01, 0x95, 0x08,
    0x05, 0x07, 0x19, 0xE0, 0x29, 0xE7, 0x15, 0x00, 0x25, 0x01,
    0x81, 0x02, 0x95, 0x01, 0x75, 0x08, 0x81, 0x03, 0x95, 0x05,
    0x75, 0x01, 0x05, 0x08, 0x19, 0x01, 0x29, 0x05, 0x91, 0x02,
    0x95, 0x01, 0x75, 0x03, 0x91, 0x03, 0x95, 0x06, 0x75, 0x08,
    0x15, 0x00, 0x25, 0x7F, 0x05, 0x07, 0x19, 0x00, 0x29, 0x7F,
    0x81, 0x00, 0xC0
};
#endif

/* Configuration descriptor: composite CDC + Keyboard, 3 interfaces, 100 bytes. */
static const uint8_t config_descriptor[CONFIG_DESC_SIZE] = {
    /* configuration (9) */
    9, 2, LSB(CONFIG_DESC_SIZE), MSB(CONFIG_DESC_SIZE), 3, 1, 0, 0xC0, 50,
    /* interface association (8) */
    8, 11, CDC_STATUS_INTERFACE, 2, 0x02, 0x02, 0x01, 0,
    /* CDC communication interface (9) */
    9, 4, CDC_STATUS_INTERFACE, 0, 1, 0x02, 0x02, 0x01, 0,
    /* CDC header functional (5) */
    5, 0x24, 0x00, 0x10, 0x01,
    /* CDC call management functional (5) */
    5, 0x24, 0x01, 0x01, CDC_DATA_INTERFACE,
    /* CDC abstract control management functional (4) */
    4, 0x24, 0x02, 0x06,
    /* CDC union functional (5) */
    5, 0x24, 0x06, CDC_STATUS_INTERFACE, CDC_DATA_INTERFACE,
    /* interrupt endpoint, EP2 IN (7) */
    7, 5, CDC_ACM_ENDPOINT | 0x80, 0x03, LSB(CDC_ACM_SIZE), MSB(CDC_ACM_SIZE), 5,
    /* CDC data interface (9) */
    9, 4, CDC_DATA_INTERFACE, 0, 2, 0x0A, 0x00, 0x00, 0,
    /* bulk OUT endpoint, EP3 OUT (7) */
    7, 5, CDC_RX_ENDPOINT, 0x02, LSB(CDC_RX_SIZE_480), MSB(CDC_RX_SIZE_480), 0,
    /* bulk IN endpoint, EP4 IN (7) */
    7, 5, CDC_TX_ENDPOINT | 0x80, 0x02, LSB(CDC_TX_SIZE_480), MSB(CDC_TX_SIZE_480), 0,
    /* keyboard interface (9) */
    9, 4, KEYBOARD_INTERFACE, 0, 1, 0x03, 0x01, 0x01, 0,
    /* HID descriptor (9) */
    9, 0x21, 0x11, 0x01, 0, 1, 0x22,
        LSB(sizeof(keyboard_report_desc)), MSB(sizeof(keyboard_report_desc)),
    /* keyboard endpoint, EP5 IN (7) */
    7, 5, KEYBOARD_ENDPOINT | 0x80, 0x03, KEYBOARD_SIZE, 0, KEYBOARD_INTERVAL
};

/* String descriptors. */
static const struct usb_string_descriptor_struct string0 = { 4, 3, {0x0409} };
static const struct usb_string_descriptor_struct string1 = { 14, 3,
    {'R','T','1','1','7','6'} };                        /* manufacturer */
static const struct usb_string_descriptor_struct string2 = { 22, 3,
    {'U','S','B',' ','S','e','r','i','a','l'} };        /* product */

const usb_descriptor_list_t usb_descriptor_list[] = {
    {0x0100, 0x0000, device_descriptor,            18},
    {0x0200, 0x0000, config_descriptor,            CONFIG_DESC_SIZE},
#if defined(KEYBOARD_INTERFACE)
    {0x2200, KEYBOARD_INTERFACE, keyboard_report_desc, sizeof(keyboard_report_desc)},
    {0x2100, KEYBOARD_INTERFACE, config_descriptor + KEYBOARD_HID_DESC_OFFSET, 9},
#endif
    {0x0300, 0x0000, (const uint8_t *)&string0,    4},
    {0x0301, 0x0409, (const uint8_t *)&string1,    14},
    {0x0302, 0x0409, (const uint8_t *)&string2,    22},
    {0x0000, 0x0000, 0,                            0}
};
