#include "block_driver.h"

int block_init(void)
{
    return 0;
}

int block_open(block_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int block_close(block_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int block_read(block_device_t *dev, block_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int block_write(block_device_t *dev, block_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}
