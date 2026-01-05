#include "block_nvme_driver.h"

int block_nvme_init(void)
{
    return 0;
}

int block_nvme_probe(void)
{
    return 0;
}

int block_nvme_read(block_nvme_device_t *dev, block_nvme_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int block_nvme_write(block_nvme_device_t *dev, block_nvme_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int block_nvme_flush(block_nvme_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int block_nvme_identify_ns(block_nvme_device_t *dev, void *buf)
{
    if (!dev || !buf)
    {
        return -1;
    }
    return 0;
}
