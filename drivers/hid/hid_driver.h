#ifndef HID_DRIVER_H
#define HID_DRIVER_H

#include <stdint.h>

#define HID_KEYBOARD 1
#define HID_MOUSE 2
#define HID_JOYSTICK 3

typedef struct
{
    uint32_t id;
    uint32_t type;
    uint32_t vendor_id;
    uint32_t product_id;
} hid_device_t;

typedef struct
{
    uint8_t *data;
    uint32_t length;
} hid_report_t;

int hid_init(void);
int hid_register_device(hid_device_t *dev);
int hid_send_report(hid_device_t *dev, hid_report_t *report);
int hid_receive_report(hid_device_t *dev, hid_report_t *report);

#endif
