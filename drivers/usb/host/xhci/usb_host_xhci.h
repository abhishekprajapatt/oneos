#ifndef USB_HOST_XHCI_H
#define USB_HOST_XHCI_H

#include <stdint.h>

typedef struct
{
    uint32_t host_id;
    uint64_t cap_base;
    uint64_t op_base;
    uint32_t max_slots;
    uint32_t flags;
} usb_host_xhci_device_t;

typedef struct
{
    uint32_t capability_regs;
    uint32_t operational_regs;
    uint32_t runtime_regs;
    uint32_t doorbell_regs;
} usb_host_xhci_regs_t;

int usb_host_xhci_init(void);
int usb_host_xhci_probe(usb_host_xhci_device_t *dev);
int usb_host_xhci_reset(usb_host_xhci_device_t *dev);
int usb_host_xhci_start(usb_host_xhci_device_t *dev);
int usb_host_xhci_stop(usb_host_xhci_device_t *dev);
int usb_host_xhci_queue_trb(usb_host_xhci_device_t *dev, uint32_t ep_id, uint64_t trb_addr);

#endif
