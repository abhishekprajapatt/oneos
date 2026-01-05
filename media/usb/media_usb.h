#ifndef MEDIA_USB_H
#define MEDIA_USB_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    const char *device_name;
    uint16_t vendor_id;
    uint16_t product_id;
    uint32_t max_packet_size;
} media_usb_device_t;

typedef struct
{
    uint32_t device_count;
    uint32_t connected_count;
    uint32_t active_transfers;
} media_usb_state_t;

int media_usb_init(void);
int media_usb_register_device(media_usb_device_t *device);
int media_usb_unregister_device(uint32_t device_id);
int media_usb_control_transfer(uint32_t device_id, uint8_t *request, uint32_t request_size);
int media_usb_bulk_transfer(uint32_t device_id, uint8_t *data, uint32_t size);
int media_usb_reset_device(uint32_t device_id);

#endif
