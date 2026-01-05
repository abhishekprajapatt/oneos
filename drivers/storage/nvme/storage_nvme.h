#ifndef STORAGE_NVME_H
#define STORAGE_NVME_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t bar0_address;
    uint32_t queue_depth;
    uint32_t namespace_count;
    uint64_t capacity;
} storage_nvme_device_t;

typedef struct
{
    uint32_t nsid;
    uint64_t size;
    uint32_t block_size;
    uint32_t features;
} storage_nvme_namespace_t;

int storage_nvme_init(void);
int storage_nvme_probe(storage_nvme_device_t *dev);
int storage_nvme_submit_command(storage_nvme_device_t *dev, uint8_t *cmd, uint8_t *response);
int storage_nvme_read(storage_nvme_device_t *dev, uint32_t nsid, uint64_t lba, uint8_t *buffer, uint32_t count);
int storage_nvme_write(storage_nvme_device_t *dev, uint32_t nsid, uint64_t lba, uint8_t *buffer, uint32_t count);
int storage_nvme_get_features(storage_nvme_device_t *dev, uint8_t feature_id, uint32_t *value);

#endif
