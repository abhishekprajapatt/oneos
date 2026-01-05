#ifndef BLOCK_ZRAM_DRIVER_H
#define BLOCK_ZRAM_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint64_t capacity;
    uint64_t used_size;
    uint32_t compress_level;
} block_zram_device_t;

typedef struct
{
    uint64_t offset;
    uint32_t length;
    void *buffer;
} block_zram_request_t;

int block_zram_init(void);
int block_zram_create(uint32_t id, uint64_t capacity);
int block_zram_destroy(uint32_t id);
int block_zram_read(block_zram_device_t *dev, block_zram_request_t *req);
int block_zram_write(block_zram_device_t *dev, block_zram_request_t *req);
int block_zram_reset(block_zram_device_t *dev);
int block_zram_get_stats(block_zram_device_t *dev, void *stats);

#endif
