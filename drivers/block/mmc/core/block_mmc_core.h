#ifndef BLOCK_MMC_CORE_H
#define BLOCK_MMC_CORE_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t capacity;
    uint32_t block_size;
    uint32_t state;
} block_mmc_device_t;

typedef struct
{
    uint64_t offset;
    uint32_t block_count;
    void *buffer;
} block_mmc_request_t;

int block_mmc_core_init(void);
int block_mmc_core_detect(void);
int block_mmc_core_read(block_mmc_device_t *dev, block_mmc_request_t *req);
int block_mmc_core_write(block_mmc_device_t *dev, block_mmc_request_t *req);
int block_mmc_core_erase(block_mmc_device_t *dev, uint64_t offset, uint32_t size);
int block_mmc_core_set_bus_width(block_mmc_device_t *dev, uint8_t width);

#endif
