#ifndef USB_HOST_EHCI_H
#define USB_HOST_EHCI_H

#include <stdint.h>

typedef struct
{
    uint32_t host_id;
    uint32_t hcc_params;
    uint32_t operational_base;
    uint32_t async_list;
    uint32_t flags;
} usb_host_ehci_device_t;

typedef struct
{
    uint32_t capability_regs;
    uint32_t operational_regs;
    uint32_t runtime_regs;
    uint32_t doorbell_regs;
} usb_host_ehci_regs_t;

int usb_host_ehci_init(void);
int usb_host_ehci_probe(usb_host_ehci_device_t *dev);
int usb_host_ehci_reset(usb_host_ehci_device_t *dev);
int usb_host_ehci_run(usb_host_ehci_device_t *dev);
int usb_host_ehci_stop(usb_host_ehci_device_t *dev);
int usb_host_ehci_submit_qtd(usb_host_ehci_device_t *dev, uint8_t *qh, uint8_t *qtd);

#endif
