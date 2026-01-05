#include "ata_driver.h"

int ata_init(void)
{
    return 0;
}

int ata_read_sectors(ata_device_t *dev, ata_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int ata_write_sectors(ata_device_t *dev, ata_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int ata_identify_device(ata_device_t *dev, void *buffer)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}
