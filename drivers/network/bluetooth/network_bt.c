#include "network_bt.h"

int network_bt_init(void)
{
    return 0;
}

int network_bt_enable_adapter(network_bt_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    dev->flags |= 1;
    return 0;
}

int network_bt_disable_adapter(network_bt_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    dev->flags &= ~1;
    return 0;
}

int network_bt_start_discovery(network_bt_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int network_bt_stop_discovery(network_bt_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int network_bt_create_connection(network_bt_device_t *dev, uint8_t *bd_addr)
{
    if (!dev || !bd_addr)
    {
        return -1;
    }
    return 0;
}

int network_bt_get_device_info(network_bt_device_t *dev, void *info)
{
    if (!dev || !info)
    {
        return -1;
    }
    return 0;
}
