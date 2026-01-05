#include "platform_driver.h"

int platform_init(void)
{
    return 0;
}

int platform_device_register(platform_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int platform_device_unregister(uint32_t id)
{
    return 0;
}

int platform_driver_register(const char *name)
{
    if (!name)
    {
        return -1;
    }
    return 0;
}

int platform_driver_unregister(const char *name)
{
    if (!name)
    {
        return -1;
    }
    return 0;
}
