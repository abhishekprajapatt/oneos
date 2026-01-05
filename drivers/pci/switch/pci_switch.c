#include "pci_switch.h"

int pci_switch_init(void)
{
    return 0;
}

int pci_switch_probe(pci_switch_device_t *sw)
{
    if (!sw)
    {
        return -1;
    }
    return 0;
}

int pci_switch_enable_port(pci_switch_device_t *sw, uint8_t port)
{
    if (!sw)
    {
        return -1;
    }
    return 0;
}

int pci_switch_disable_port(pci_switch_device_t *sw, uint8_t port)
{
    if (!sw)
    {
        return -1;
    }
    return 0;
}

int pci_switch_set_bridge_control(pci_switch_device_t *sw, uint32_t control)
{
    if (!sw)
    {
        return -1;
    }
    return 0;
}

int pci_switch_get_port_status(pci_switch_device_t *sw, uint8_t port, uint32_t *status)
{
    if (!sw || !status)
    {
        return -1;
    }
    return 0;
}
