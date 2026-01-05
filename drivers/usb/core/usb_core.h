#ifndef USB_CORE_H
#define USB_CORE_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint16_t vendor_id;
    uint16_t product_id;
    uint8_t bus;
    uint8_t dev;
    uint8_t usb_version;
} usb_core_device_t;

typedef struct
{
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
} usb_core_descriptor_t;

int usb_core_init(void);
int usb_core_register_device(usb_core_device_t *dev);
int usb_core_unregister_device(usb_core_device_t *dev);
int usb_core_control_transfer(usb_core_device_t *dev, uint8_t *request, uint8_t *data);
int usb_core_bulk_transfer(usb_core_device_t *dev, uint8_t endpoint, uint8_t *data, uint32_t len);
int usb_core_get_descriptor(usb_core_device_t *dev, uint8_t type, uint8_t index, uint8_t *buffer, uint16_t len);

#endif
