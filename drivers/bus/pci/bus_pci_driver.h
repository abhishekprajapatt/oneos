#ifndef BUS_PCI_DRIVER_H
#define BUS_PCI_DRIVER_H

#include <stdint.h>

#define BUS_PCI_CLASS_DISPLAY 0x03
#define BUS_PCI_CLASS_NETWORK 0x02
#define BUS_PCI_CLASS_STORAGE 0x01

typedef struct
{
    uint8_t bus;
    uint8_t slot;
    uint8_t func;
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t class_code;
} bus_pci_device_t;

typedef struct
{
    uint32_t reg;
    uint32_t value;
} bus_pci_config_t;

int bus_pci_init(void);
int bus_pci_scan(void);
int bus_pci_read_config(bus_pci_device_t *dev, bus_pci_config_t *cfg);
int bus_pci_write_config(bus_pci_device_t *dev, bus_pci_config_t *cfg);
int bus_pci_enable_device(bus_pci_device_t *dev);
int bus_pci_disable_device(bus_pci_device_t *dev);

#endif
