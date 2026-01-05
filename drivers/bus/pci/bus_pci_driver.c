#include "bus_pci_driver.h"

int bus_pci_init(void)
{
    return 0;
}

int bus_pci_scan(void)
{
    return 0;
}

int bus_pci_read_config(bus_pci_device_t *dev, bus_pci_config_t *cfg)
{
    if (!dev || !cfg)
    {
        return -1;
    }
    return 0;
}

int bus_pci_write_config(bus_pci_device_t *dev, bus_pci_config_t *cfg)
{
    if (!dev || !cfg)
    {
        return -1;
    }
    return 0;
}

int bus_pci_enable_device(bus_pci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int bus_pci_disable_device(bus_pci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
