#include "net_eth_mellanox.h"

int net_eth_mellanox_init(void)
{
    return 0;
}

int net_eth_mellanox_probe(void)
{
    return 0;
}

int net_eth_mellanox_open(net_eth_mellanox_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_eth_mellanox_close(net_eth_mellanox_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_eth_mellanox_set_params(net_eth_mellanox_device_t *dev, net_eth_mellanox_params_t *params)
{
    if (!dev || !params)
    {
        return -1;
    }
    return 0;
}

int net_eth_mellanox_query_status(net_eth_mellanox_device_t *dev, void *status)
{
    if (!dev || !status)
    {
        return -1;
    }
    return 0;
}
