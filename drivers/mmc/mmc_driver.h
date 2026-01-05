#ifndef MMC_DRIVER_H
#define MMC_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint64_t capacity;
    uint32_t block_size;
    uint32_t state;
} mmc_device_t;

typedef struct
{
    uint64_t offset;
    uint32_t block_count;
    void *buffer;
} mmc_request_t;

int mmc_init(void);
int mmc_detect(void);
int mmc_read(mmc_device_t *dev, mmc_request_t *req);
int mmc_write(mmc_device_t *dev, mmc_request_t *req);
int mmc_erase(mmc_device_t *dev, uint64_t offset, uint32_t size);

#endif
