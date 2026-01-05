#include "net_wl_realtek.h"

int net_wl_realtek_init(void)
{
    return 0;
}

int net_wl_realtek_probe(void)
{
    return 0;
}

int net_wl_realtek_open(net_wl_realtek_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_wl_realtek_close(net_wl_realtek_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_wl_realtek_associate(net_wl_realtek_device_t *dev, net_wl_realtek_assoc_t *assoc)
{
    if (!dev || !assoc)
    {
        return -1;
    }
    return 0;
}

int net_wl_realtek_disassociate(net_wl_realtek_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
