#include "usb_host_ehci.h"

int usb_host_ehci_init(void)
{
    return 0;
}

int usb_host_ehci_probe(usb_host_ehci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_host_ehci_reset(usb_host_ehci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_host_ehci_run(usb_host_ehci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_host_ehci_stop(usb_host_ehci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_host_ehci_submit_qtd(usb_host_ehci_device_t *dev, uint8_t *qh, uint8_t *qtd)
{
    if (!dev || !qh || !qtd)
    {
        return -1;
    }
    return 0;
}
