#include "network_driver.h"

int network_init(void)
{
    return 0;
}

int network_configure(network_device_t *dev, network_config_t *cfg)
{
    if (!dev || !cfg)
    {
        return -1;
    }
    dev->config = *cfg;
    return 0;
}

int network_up(network_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    dev->flags |= 1;
    return 0;
}

int network_down(network_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    dev->flags &= ~1;
    return 0;
}

int network_get_config(network_device_t *dev, network_config_t *cfg)
{
    if (!dev || !cfg)
    {
        return -1;
    }
    *cfg = dev->config;
    return 0;
}
