#include "usb_host_xhci.h"

int usb_host_xhci_init(void)
{
    return 0;
}

int usb_host_xhci_probe(usb_host_xhci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_host_xhci_reset(usb_host_xhci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_host_xhci_start(usb_host_xhci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_host_xhci_stop(usb_host_xhci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_host_xhci_queue_trb(usb_host_xhci_device_t *dev, uint32_t ep_id, uint64_t trb_addr)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
