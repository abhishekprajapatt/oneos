#include "net_eth_realtek.h"

int net_eth_realtek_init(void)
{
    return 0;
}

int net_eth_realtek_probe(void)
{
    return 0;
}

int net_eth_realtek_open(net_eth_realtek_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_eth_realtek_close(net_eth_realtek_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_eth_realtek_transmit(net_eth_realtek_device_t *dev, void *packet, uint32_t len)
{
    if (!dev || !packet || len == 0)
    {
        return -1;
    }
    return 0;
}

int net_eth_realtek_get_stats(net_eth_realtek_device_t *dev, net_eth_realtek_stats_t *stats)
{
    if (!dev || !stats)
    {
        return -1;
    }
    return 0;
}
