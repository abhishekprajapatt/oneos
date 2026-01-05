#include "pci_driver.h"

int pci_init(void)
{
    return 0;
}

int pci_scan_bus(void)
{
    return 0;
}

int pci_read_config(pci_device_t *dev, pci_config_t *cfg)
{
    if (!dev || !cfg)
    {
        return -1;
    }
    return 0;
}

int pci_write_config(pci_device_t *dev, pci_config_t *cfg)
{
    if (!dev || !cfg)
    {
        return -1;
    }
    return 0;
}

int pci_enable_device(pci_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
