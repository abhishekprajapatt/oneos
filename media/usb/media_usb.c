#include "media_usb.h"

int media_usb_init(void)
{
    return 0;
}

int media_usb_register_device(media_usb_device_t *device)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}

int media_usb_unregister_device(uint32_t device_id)
{
    return 0;
}

int media_usb_control_transfer(uint32_t device_id, uint8_t *request, uint32_t request_size)
{
    if (!request)
    {
        return -1;
    }
    return 0;
}

int media_usb_bulk_transfer(uint32_t device_id, uint8_t *data, uint32_t size)
{
    if (!data)
    {
        return -1;
    }
    return 0;
}

int media_usb_reset_device(uint32_t device_id)
{
    return 0;
}
