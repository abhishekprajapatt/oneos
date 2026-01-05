#include "storage_ata.h"

int storage_ata_init(void)
{
    return 0;
}

int storage_ata_probe(storage_ata_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int storage_ata_identify(storage_ata_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int storage_ata_read_sector(storage_ata_device_t *dev, uint32_t lba, uint8_t *buffer, uint32_t count)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}

int storage_ata_write_sector(storage_ata_device_t *dev, uint32_t lba, uint8_t *buffer, uint32_t count)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}

int storage_ata_set_power_mode(storage_ata_device_t *dev, uint8_t mode)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
