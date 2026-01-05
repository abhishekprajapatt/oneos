#include "char_misc_driver.h"

int char_misc_init(void)
{
    return 0;
}

int char_misc_register(char_misc_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int char_misc_unregister(uint32_t device_id)
{
    return 0;
}

int char_misc_open(char_misc_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int char_misc_close(char_misc_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int char_misc_ioctl(char_misc_device_t *dev, uint32_t cmd, void *arg)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
