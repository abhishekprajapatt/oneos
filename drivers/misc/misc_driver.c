#include "misc_driver.h"

int misc_init(void)
{
    return 0;
}

int misc_register(misc_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int misc_unregister(uint32_t id)
{
    return 0;
}

int misc_open(misc_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int misc_close(misc_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
