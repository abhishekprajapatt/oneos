#include "usb_storage.h"

int usb_storage_init(void)
{
    return 0;
}

int usb_storage_probe(usb_storage_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_storage_test_unit_ready(usb_storage_device_t *dev, uint8_t lun)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int usb_storage_read_capacity(usb_storage_device_t *dev, uint8_t lun, uint64_t *blocks, uint32_t *block_size)
{
    if (!dev || !blocks || !block_size)
    {
        return -1;
    }
    return 0;
}

int usb_storage_read(usb_storage_device_t *dev, uint8_t lun, uint64_t lba, uint8_t *buffer, uint32_t count)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}

int usb_storage_write(usb_storage_device_t *dev, uint8_t lun, uint64_t lba, uint8_t *buffer, uint32_t count)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}
