#include "storage_nvme.h"

int storage_nvme_init(void)
{
    return 0;
}

int storage_nvme_probe(storage_nvme_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int storage_nvme_submit_command(storage_nvme_device_t *dev, uint8_t *cmd, uint8_t *response)
{
    if (!dev || !cmd || !response)
    {
        return -1;
    }
    return 0;
}

int storage_nvme_read(storage_nvme_device_t *dev, uint32_t nsid, uint64_t lba, uint8_t *buffer, uint32_t count)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}

int storage_nvme_write(storage_nvme_device_t *dev, uint32_t nsid, uint64_t lba, uint8_t *buffer, uint32_t count)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}

int storage_nvme_get_features(storage_nvme_device_t *dev, uint8_t feature_id, uint32_t *value)
{
    if (!dev || !value)
    {
        return -1;
    }
    return 0;
}
