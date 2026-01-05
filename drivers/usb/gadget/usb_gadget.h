#ifndef USB_GADGET_H
#define USB_GADGET_H

#include <stdint.h>

typedef struct
{
    uint32_t gadget_id;
    uint32_t flags;
    uint8_t state;
    uint16_t vendor_id;
    uint16_t product_id;
} usb_gadget_device_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
} usb_gadget_ep_descriptor_t;

int usb_gadget_init(void);
int usb_gadget_register(usb_gadget_device_t *dev);
int usb_gadget_unregister(usb_gadget_device_t *dev);
int usb_gadget_enable_ep(usb_gadget_device_t *dev, uint8_t ep);
int usb_gadget_disable_ep(usb_gadget_device_t *dev, uint8_t ep);
int usb_gadget_queue_request(usb_gadget_device_t *dev, uint8_t ep, uint8_t *data, uint32_t len);

#endif
