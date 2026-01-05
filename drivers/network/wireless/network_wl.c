#include "network_wl.h"

int network_wl_init(void)
{
    return 0;
}

int network_wl_enable(network_wl_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int network_wl_disable(network_wl_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int network_wl_scan(network_wl_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int network_wl_connect(network_wl_device_t *dev, uint8_t *ssid, uint32_t ssid_len, uint8_t *password, uint32_t pass_len)
{
    if (!dev || !ssid || ssid_len == 0)
    {
        return -1;
    }
    return 0;
}

int network_wl_disconnect(network_wl_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int network_wl_set_power_save(network_wl_device_t *dev, uint32_t mode)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
