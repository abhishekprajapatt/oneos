#include "nvme_driver.h"

int nvme_init(void)
{
    return 0;
}

int nvme_probe(void)
{
    return 0;
}

int nvme_read(nvme_device_t *dev, nvme_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int nvme_write(nvme_device_t *dev, nvme_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int nvme_identify(nvme_device_t *dev, void *buffer)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}
