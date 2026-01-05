#include "mmc_driver.h"

int mmc_init(void)
{
    return 0;
}

int mmc_detect(void)
{
    return 0;
}

int mmc_read(mmc_device_t *dev, mmc_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int mmc_write(mmc_device_t *dev, mmc_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int mmc_erase(mmc_device_t *dev, uint64_t offset, uint32_t size)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
