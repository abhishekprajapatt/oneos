#ifndef BLOCK_DRIVER_H
#define BLOCK_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint64_t block_size;
    uint64_t total_blocks;
} block_device_t;

typedef struct
{
    uint64_t block_offset;
    uint32_t block_count;
    void *buffer;
} block_request_t;

int block_init(void);
int block_open(block_device_t *dev);
int block_close(block_device_t *dev);
int block_read(block_device_t *dev, block_request_t *req);
int block_write(block_device_t *dev, block_request_t *req);

#endif
