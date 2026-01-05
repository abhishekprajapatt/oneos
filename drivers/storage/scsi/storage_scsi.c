#include "storage_scsi.h"

int storage_scsi_init(void)
{
    return 0;
}

int storage_scsi_probe(storage_scsi_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int storage_scsi_inquiry(storage_scsi_device_t *dev, storage_scsi_inquiry_t *info)
{
    if (!dev || !info)
    {
        return -1;
    }
    return 0;
}

int storage_scsi_read_capacity(storage_scsi_device_t *dev, uint64_t *block_count, uint32_t *block_size)
{
    if (!dev || !block_count || !block_size)
    {
        return -1;
    }
    return 0;
}

int storage_scsi_read(storage_scsi_device_t *dev, uint64_t lba, uint8_t *buffer, uint32_t count)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}

int storage_scsi_write(storage_scsi_device_t *dev, uint64_t lba, uint8_t *buffer, uint32_t count)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}
