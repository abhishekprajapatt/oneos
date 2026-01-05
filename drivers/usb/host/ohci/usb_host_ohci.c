#include "usb_host_ohci.h"

int usb_host_ohci_init(void)
{
    return 0;
}

int usb_host_ohci_probe(usb_host_ohci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_host_ohci_reset(usb_host_ohci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_host_ohci_start(usb_host_ohci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_host_ohci_stop(usb_host_ohci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_host_ohci_queue_ed(usb_host_ohci_device_t *dev, uint8_t *ed, uint32_t head_addr)
{
    if (!dev || !ed)
    {
        return -1;
    }
    return 0;
}
