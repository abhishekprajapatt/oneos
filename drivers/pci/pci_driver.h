#ifndef PCI_DRIVER_H
#define PCI_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t bus;
    uint8_t slot;
    uint8_t function;
} pci_device_t;

typedef struct
{
    uint32_t reg;
    uint32_t value;
} pci_config_t;

int pci_init(void);
int pci_scan_bus(void);
int pci_read_config(pci_device_t *dev, pci_config_t *cfg);
int pci_write_config(pci_device_t *dev, pci_config_t *cfg);
int pci_enable_device(pci_device_t *dev);

#endif
