#ifndef USB_MISC_H
#define USB_MISC_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint16_t vendor_id;
    uint16_t product_id;
    uint32_t flags;
    uint8_t control_index;
} usb_misc_device_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wValue;
    uint16_t wIndex;
} usb_misc_control_request_t;

int usb_misc_init(void);
int usb_misc_probe(usb_misc_device_t *dev);
int usb_misc_set_feature(usb_misc_device_t *dev, uint16_t feature);
int usb_misc_clear_feature(usb_misc_device_t *dev, uint16_t feature);
int usb_misc_get_status(usb_misc_device_t *dev, uint8_t recipient, uint16_t *status);
int usb_misc_set_descriptor(usb_misc_device_t *dev, uint8_t type, uint8_t *data, uint16_t len);

#endif
