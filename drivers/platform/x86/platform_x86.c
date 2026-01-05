#include "platform_x86.h"

int platform_x86_init(void)
{
    return 0;
}

int platform_x86_probe(platform_x86_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int platform_x86_enable(platform_x86_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int platform_x86_disable(platform_x86_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int platform_x86_get_resources(platform_x86_device_t *dev, platform_x86_resource_t *res)
{
    if (!dev || !res)
    {
        return -1;
    }
    return 0;
}

int platform_x86_set_config(platform_x86_device_t *dev, uint32_t *config, uint32_t size)
{
    if (!dev || !config)
    {
        return -1;
    }
    return 0;
}
