#ifndef PCI_HOST_H
#define PCI_HOST_H

#include <stdint.h>

typedef struct
{
    uint32_t host_id;
    uint32_t segment;
    uint8_t start_bus;
    uint8_t end_bus;
    uint32_t mmio_base;
    uint32_t mmio_size;
} pci_host_device_t;

typedef struct
{
    uint16_t vendor_id;
    uint16_t device_id;
    uint32_t class_code;
    uint8_t bus;
    uint8_t slot;
    uint8_t func;
} pci_host_device_info_t;

int pci_host_init(void);
int pci_host_add_bridge(pci_host_device_t *host);
int pci_host_remove_bridge(pci_host_device_t *host);
int pci_host_scan_bus(pci_host_device_t *host, uint8_t bus_num);
int pci_host_config_read(pci_host_device_t *host, uint8_t bus, uint8_t slot, uint8_t func, uint32_t offset, uint32_t *value);
int pci_host_config_write(pci_host_device_t *host, uint8_t bus, uint8_t slot, uint8_t func, uint32_t offset, uint32_t value);

#endif
