#include "pci_host.h"

int pci_host_init(void)
{
    return 0;
}

int pci_host_add_bridge(pci_host_device_t *host)
{
    if (!host)
    {
        return -1;
    }
    return 0;
}

int pci_host_remove_bridge(pci_host_device_t *host)
{
    if (!host)
    {
        return -1;
    }
    return 0;
}

int pci_host_scan_bus(pci_host_device_t *host, uint8_t bus_num)
{
    if (!host)
    {
        return -1;
    }
    return 0;
}

int pci_host_config_read(pci_host_device_t *host, uint8_t bus, uint8_t slot, uint8_t func, uint32_t offset, uint32_t *value)
{
    if (!host || !value)
    {
        return -1;
    }
    return 0;
}

int pci_host_config_write(pci_host_device_t *host, uint8_t bus, uint8_t slot, uint8_t func, uint32_t offset, uint32_t value)
{
    if (!host)
    {
        return -1;
    }
    return 0;
}
