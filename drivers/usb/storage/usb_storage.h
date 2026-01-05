#ifndef USB_STORAGE_H
#define USB_STORAGE_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint16_t vendor_id;
    uint16_t product_id;
    uint32_t lun_count;
    uint64_t capacity;
} usb_storage_device_t;

typedef struct
{
    uint32_t tag;
    uint32_t data_len;
    uint8_t direction;
    uint8_t lun;
    uint8_t cmd_len;
} usb_storage_cbw_t;

int usb_storage_init(void);
int usb_storage_probe(usb_storage_device_t *dev);
int usb_storage_test_unit_ready(usb_storage_device_t *dev, uint8_t lun);
int usb_storage_read_capacity(usb_storage_device_t *dev, uint8_t lun, uint64_t *blocks, uint32_t *block_size);
int usb_storage_read(usb_storage_device_t *dev, uint8_t lun, uint64_t lba, uint8_t *buffer, uint32_t count);
int usb_storage_write(usb_storage_device_t *dev, uint8_t lun, uint64_t lba, uint8_t *buffer, uint32_t count);

#endif
