#include "block_mmc_core.h"

int block_mmc_core_init(void)
{
    return 0;
}

int block_mmc_core_detect(void)
{
    return 0;
}

int block_mmc_core_read(block_mmc_device_t *dev, block_mmc_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int block_mmc_core_write(block_mmc_device_t *dev, block_mmc_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int block_mmc_core_erase(block_mmc_device_t *dev, uint64_t offset, uint32_t size)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int block_mmc_core_set_bus_width(block_mmc_device_t *dev, uint8_t width)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
