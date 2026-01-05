#ifndef BLOCK_NVME_DRIVER_H
#define BLOCK_NVME_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t nsid;
    uint64_t capacity;
    uint32_t block_size;
} block_nvme_device_t;

typedef struct
{
    uint64_t lba;
    uint32_t count;
    void *buffer;
    uint32_t flags;
} block_nvme_request_t;

int block_nvme_init(void);
int block_nvme_probe(void);
int block_nvme_read(block_nvme_device_t *dev, block_nvme_request_t *req);
int block_nvme_write(block_nvme_device_t *dev, block_nvme_request_t *req);
int block_nvme_flush(block_nvme_device_t *dev);
int block_nvme_identify_ns(block_nvme_device_t *dev, void *buf);

#endif
