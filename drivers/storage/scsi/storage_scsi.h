#ifndef STORAGE_SCSI_H
#define STORAGE_SCSI_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint8_t host_scsi_id;
    uint8_t target_id;
    uint8_t lun;
    uint32_t flags;
} storage_scsi_device_t;

typedef struct
{
    uint32_t vendor_id;
    uint32_t product_id;
    uint32_t revision;
    uint64_t block_count;
    uint32_t block_size;
} storage_scsi_inquiry_t;

int storage_scsi_init(void);
int storage_scsi_probe(storage_scsi_device_t *dev);
int storage_scsi_inquiry(storage_scsi_device_t *dev, storage_scsi_inquiry_t *info);
int storage_scsi_read_capacity(storage_scsi_device_t *dev, uint64_t *block_count, uint32_t *block_size);
int storage_scsi_read(storage_scsi_device_t *dev, uint64_t lba, uint8_t *buffer, uint32_t count);
int storage_scsi_write(storage_scsi_device_t *dev, uint64_t lba, uint8_t *buffer, uint32_t count);

#endif
