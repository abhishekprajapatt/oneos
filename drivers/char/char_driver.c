#include "char_driver.h"

int char_init(void)
{
    return 0;
}

int char_open(char_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int char_close(char_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int char_read(char_device_t *dev, char_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int char_write(char_device_t *dev, char_request_t *req)
{
    if (!dev || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int char_ioctl(char_device_t *dev, uint32_t cmd, void *arg)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
