#ifndef USB_DRIVER_H
#define USB_DRIVER_H

#include <stdint.h>

#define USB_SPEED_LOW 0
#define USB_SPEED_FULL 1
#define USB_SPEED_HIGH 2
#define USB_SPEED_SUPER 3

typedef struct
{
    uint32_t device_id;
    uint16_t vendor_id;
    uint16_t product_id;
    uint8_t speed;
} usb_device_t;

typedef struct
{
    uint8_t endpoint;
    uint8_t *buffer;
    uint32_t length;
    uint32_t flags;
} usb_request_t;

int usb_init(void);
int usb_host_init(void);
int usb_device_init(void);
int usb_enumerate(void);
int usb_submit_urb(usb_device_t *dev, usb_request_t *req);

#endif
