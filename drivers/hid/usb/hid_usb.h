#ifndef HID_USB_H
#define HID_USB_H

#include <stdint.h>

typedef struct
{
    uint32_t hid_id;
    uint16_t vendor_id;
    uint16_t product_id;
    uint8_t report_id;
    uint32_t flags;
} hid_usb_device_t;

typedef struct
{
    uint8_t *descriptor;
    uint32_t length;
    uint8_t type;
} hid_usb_descriptor_t;

int hid_usb_init(void);
int hid_usb_probe(hid_usb_device_t *dev);
int hid_usb_get_descriptor(hid_usb_device_t *dev, hid_usb_descriptor_t *desc);
int hid_usb_send_report(hid_usb_device_t *dev, uint8_t *report, uint32_t len);
int hid_usb_receive_report(hid_usb_device_t *dev, uint8_t *report, uint32_t len);
int hid_usb_set_idle(hid_usb_device_t *dev, uint8_t duration);

#endif
