#include "sound_usb.h"

int sound_usb_init(void)
{
    return 0;
}

int sound_usb_probe(sound_usb_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int sound_usb_set_format(sound_usb_device_t *dev, sound_usb_format_t *fmt)
{
    if (!dev || !fmt)
    {
        return -1;
    }
    return 0;
}

int sound_usb_start_stream(sound_usb_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int sound_usb_stop_stream(sound_usb_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int sound_usb_submit_urb(sound_usb_device_t *dev, uint8_t *data, uint32_t len)
{
    if (!dev || !data)
    {
        return -1;
    }
    return 0;
}
