#include "block_zram_driver.h"

int block_zram_init(void)
{
    return 0;
}

int block_zram_create(uint32_t id, uint64_t capacity)
{
    return 0;
}

int block_zram_destroy(uint32_t id)
{
    return 0;
}

int block_zram_read(block_zram_device_t *dev, block_zram_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int block_zram_write(block_zram_device_t *dev, block_zram_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int block_zram_reset(block_zram_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    dev->used_size = 0;
    return 0;
}

int block_zram_get_stats(block_zram_device_t *dev, void *stats)
{
    if (!dev || !stats)
    {
        return -1;
    }
    return 0;
}
