#include "block_sata_driver.h"

int block_sata_init(void)
{
    return 0;
}

int block_sata_probe(void)
{
    return 0;
}

int block_sata_phy_up(block_sata_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    dev->flags |= 1;
    return 0;
}

int block_sata_read(block_sata_device_t *dev, block_sata_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int block_sata_write(block_sata_device_t *dev, block_sata_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int block_sata_flush(block_sata_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
