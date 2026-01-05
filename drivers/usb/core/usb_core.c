#include "usb_core.h"

int usb_core_init(void)
{
    return 0;
}

int usb_core_register_device(usb_core_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_core_unregister_device(usb_core_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_core_control_transfer(usb_core_device_t *dev, uint8_t *request, uint8_t *data)
{
    if (!dev || !request)
    {
        return -1;
    }
    return 0;
}

int usb_core_bulk_transfer(usb_core_device_t *dev, uint8_t endpoint, uint8_t *data, uint32_t len)
{
    if (!dev || !data)
    {
        return -1;
    }
    return 0;
}

int usb_core_get_descriptor(usb_core_device_t *dev, uint8_t type, uint8_t index, uint8_t *buffer, uint16_t len)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}
