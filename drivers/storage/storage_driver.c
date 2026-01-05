#include "storage_driver.h"

int storage_init(void)
{
    return 0;
}

int storage_detect(void)
{
    return 0;
}

int storage_read(storage_device_t *dev, storage_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int storage_write(storage_device_t *dev, storage_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int storage_flush(storage_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
