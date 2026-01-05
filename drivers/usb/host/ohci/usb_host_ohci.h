#ifndef USB_HOST_OHCI_H
#define USB_HOST_OHCI_H

#include <stdint.h>

typedef struct
{
    uint32_t host_id;
    uint32_t regs_base;
    uint32_t hcca_base;
    uint32_t control_ed;
    uint32_t flags;
} usb_host_ohci_device_t;

typedef struct
{
    uint32_t revision;
    uint32_t control;
    uint32_t command_status;
    uint32_t interrupt_status;
} usb_host_ohci_regs_t;

int usb_host_ohci_init(void);
int usb_host_ohci_probe(usb_host_ohci_device_t *dev);
int usb_host_ohci_reset(usb_host_ohci_device_t *dev);
int usb_host_ohci_start(usb_host_ohci_device_t *dev);
int usb_host_ohci_stop(usb_host_ohci_device_t *dev);
int usb_host_ohci_queue_ed(usb_host_ohci_device_t *dev, uint8_t *ed, uint32_t head_addr);

#endif
