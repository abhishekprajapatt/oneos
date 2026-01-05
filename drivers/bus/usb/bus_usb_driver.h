#ifndef BUS_USB_DRIVER_H
#define BUS_USB_DRIVER_H

#include <stdint.h>

#define BUS_USB_SPEED_LOW 0
#define BUS_USB_SPEED_FULL 1
#define BUS_USB_SPEED_HIGH 2
#define BUS_USB_SPEED_SUPER 3

typedef struct
{
    uint32_t device_id;
    uint8_t bus_num;
    uint8_t dev_addr;
    uint16_t vendor_id;
    uint16_t product_id;
    uint8_t speed;
} bus_usb_device_t;

typedef struct
{
    uint8_t endpoint;
    uint8_t *buffer;
    uint32_t length;
    uint32_t flags;
} bus_usb_request_t;

int bus_usb_init(void);
int bus_usb_host_init(void);
int bus_usb_enumerate(void);
int bus_usb_reset_device(bus_usb_device_t *dev);
int bus_usb_submit_request(bus_usb_device_t *dev, bus_usb_request_t *req);
int bus_usb_get_descriptor(bus_usb_device_t *dev, uint8_t type, uint8_t *buffer, uint32_t len);

#endif
