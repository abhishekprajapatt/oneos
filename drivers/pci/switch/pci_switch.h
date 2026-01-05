#ifndef PCI_SWITCH_H
#define PCI_SWITCH_H

#include <stdint.h>

typedef struct
{
    uint32_t switch_id;
    uint16_t vendor_id;
    uint16_t device_id;
    uint8_t upstream_bus;
    uint8_t downstream_bus;
    uint32_t flags;
} pci_switch_device_t;

typedef struct
{
    uint8_t downstream_port;
    uint8_t downstream_bus;
    uint32_t secondary_status;
    uint32_t subordinate_bus;
} pci_switch_port_t;

int pci_switch_init(void);
int pci_switch_probe(pci_switch_device_t *sw);
int pci_switch_enable_port(pci_switch_device_t *sw, uint8_t port);
int pci_switch_disable_port(pci_switch_device_t *sw, uint8_t port);
int pci_switch_set_bridge_control(pci_switch_device_t *sw, uint32_t control);
int pci_switch_get_port_status(pci_switch_device_t *sw, uint8_t port, uint32_t *status);

#endif
