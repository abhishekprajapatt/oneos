#include "block_pata_driver.h"

int block_pata_init(void)
{
    return 0;
}

int block_pata_probe(void)
{
    return 0;
}

int block_pata_read(block_pata_device_t *dev, block_pata_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int block_pata_write(block_pata_device_t *dev, block_pata_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int block_pata_identify(block_pata_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int block_pata_set_dma(block_pata_device_t *dev, uint32_t enable)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
