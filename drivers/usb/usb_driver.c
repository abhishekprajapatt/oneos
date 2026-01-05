#include "usb_driver.h"

int usb_init(void)
{
    return 0;
}

int usb_host_init(void)
{
    return 0;
}

int usb_device_init(void)
{
    return 0;
}

int usb_enumerate(void)
{
    return 0;
}

int usb_submit_urb(usb_device_t *dev, usb_request_t *req)
{
    if (!dev || !req)
    {
        return -1;
    }
    return 0;
}
