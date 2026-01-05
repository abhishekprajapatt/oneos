#include "bus_platform_driver.h"

int bus_platform_init(void)
{
    return 0;
}

int bus_platform_device_register(bus_platform_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int bus_platform_device_unregister(uint32_t id)
{
    return 0;
}

int bus_platform_driver_register(bus_platform_driver_t *driver)
{
    if (!driver)
    {
        return -1;
    }
    return 0;
}

int bus_platform_driver_unregister(const char *name)
{
    if (!name)
    {
        return -1;
    }
    return 0;
}

int bus_platform_add_resources(bus_platform_device_t *dev, uint32_t *res, uint32_t count)
{
    if (!dev || !res)
    {
        return -1;
    }
    dev->resource = res;
    dev->num_resources = count;
    return 0;
}
