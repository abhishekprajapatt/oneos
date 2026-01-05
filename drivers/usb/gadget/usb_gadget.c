#include "usb_gadget.h"

int usb_gadget_init(void)
{
    return 0;
}

int usb_gadget_register(usb_gadget_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_gadget_unregister(usb_gadget_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_gadget_enable_ep(usb_gadget_device_t *dev, uint8_t ep)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_gadget_disable_ep(usb_gadget_device_t *dev, uint8_t ep)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_gadget_queue_request(usb_gadget_device_t *dev, uint8_t ep, uint8_t *data, uint32_t len)
{
    if (!dev || !data)
    {
        return -1;
    }
    return 0;
}
