#ifndef NVME_DRIVER_H
#define NVME_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint64_t namespace_size;
    uint32_t block_size;
} nvme_device_t;

typedef struct
{
    uint64_t lba;
    uint32_t count;
    void *buffer;
    uint32_t opcode;
} nvme_request_t;

int nvme_init(void);
int nvme_probe(void);
int nvme_read(nvme_device_t *dev, nvme_request_t *req);
int nvme_write(nvme_device_t *dev, nvme_request_t *req);
int nvme_identify(nvme_device_t *dev, void *buffer);

#endif
