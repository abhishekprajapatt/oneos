#include "net_eth_marvell.h"

int net_eth_marvell_init(void)
{
    return 0;
}

int net_eth_marvell_probe(void)
{
    return 0;
}

int net_eth_marvell_open(net_eth_marvell_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_eth_marvell_close(net_eth_marvell_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_eth_marvell_configure(net_eth_marvell_device_t *dev, net_eth_marvell_config_t *cfg)
{
    if (!dev || !cfg)
    {
        return -1;
    }
    return 0;
}

int net_eth_marvell_transmit(net_eth_marvell_device_t *dev, void *packet, uint32_t len)
{
    if (!dev || !packet || len == 0)
    {
        return -1;
    }
    return 0;
}
