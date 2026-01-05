#include "storage_mmc.h"

int storage_mmc_init(void)
{
    return 0;
}

int storage_mmc_probe(storage_mmc_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int storage_mmc_detect_card(storage_mmc_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int storage_mmc_read_block(storage_mmc_device_t *dev, uint32_t block, uint8_t *buffer)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}

int storage_mmc_write_block(storage_mmc_device_t *dev, uint32_t block, uint8_t *buffer)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}

int storage_mmc_erase_blocks(storage_mmc_device_t *dev, uint32_t start, uint32_t count)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
