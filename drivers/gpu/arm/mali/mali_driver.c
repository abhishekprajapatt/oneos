#include "mali_driver.h"

int mali_init(void)
{
    return 0;
}

int mali_probe(void)
{
    return 0;
}

int mali_open(mali_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int mali_close(mali_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int mali_get_core_info(mali_device_t *dev, mali_core_info_t *info)
{
    if (!dev || !info)
    {
        return -1;
    }
    return 0;
}

int mali_set_frequency(mali_device_t *dev, uint32_t freq)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
