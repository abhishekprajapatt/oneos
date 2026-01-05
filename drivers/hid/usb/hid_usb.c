#include "hid_usb.h"

int hid_usb_init(void)
{
    return 0;
}

int hid_usb_probe(hid_usb_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int hid_usb_get_descriptor(hid_usb_device_t *dev, hid_usb_descriptor_t *desc)
{
    if (!dev || !desc)
    {
        return -1;
    }
    return 0;
}

int hid_usb_send_report(hid_usb_device_t *dev, uint8_t *report, uint32_t len)
{
    if (!dev || !report)
    {
        return -1;
    }
    return 0;
}

int hid_usb_receive_report(hid_usb_device_t *dev, uint8_t *report, uint32_t len)
{
    if (!dev || !report)
    {
        return -1;
    }
    return 0;
}

int hid_usb_set_idle(hid_usb_device_t *dev, uint8_t duration)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
