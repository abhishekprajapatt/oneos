#include "bus_usb_driver.h"

int bus_usb_init(void)
{
    return 0;
}

int bus_usb_host_init(void)
{
    return 0;
}

int bus_usb_enumerate(void)
{
    return 0;
}

int bus_usb_reset_device(bus_usb_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int bus_usb_submit_request(bus_usb_device_t *dev, bus_usb_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int bus_usb_get_descriptor(bus_usb_device_t *dev, uint8_t type, uint8_t *buffer, uint32_t len)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}
