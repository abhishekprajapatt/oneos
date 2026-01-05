#include "tty_driver.h"

int tty_init(void)
{
    return 0;
}

int tty_register_device(tty_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int tty_write(tty_device_t *dev, tty_buffer_t *buf)
{
    if (!dev || !buf || !buf->buffer)
    {
        return -1;
    }
    return 0;
}

int tty_read(tty_device_t *dev, tty_buffer_t *buf)
{
    if (!dev || !buf || !buf->buffer)
    {
        return -1;
    }
    return 0;
}

int tty_ioctl(tty_device_t *dev, uint32_t cmd, void *arg)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
