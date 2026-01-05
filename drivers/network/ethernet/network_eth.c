#include "network_eth.h"

int network_eth_init(void)
{
    return 0;
}

int network_eth_configure(network_eth_device_t *dev, network_eth_ipconfig_t *cfg)
{
    if (!dev || !cfg)
    {
        return -1;
    }
    return 0;
}

int network_eth_up(network_eth_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    dev->flags |= 1;
    return 0;
}

int network_eth_down(network_eth_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    dev->flags &= ~1;
    return 0;
}

int network_eth_transmit(network_eth_device_t *dev, network_eth_packet_t *pkt)
{
    if (!dev || !pkt || !pkt->data)
    {
        return -1;
    }
    return 0;
}

int network_eth_receive(network_eth_device_t *dev, network_eth_packet_t *pkt)
{
    if (!dev || !pkt || !pkt->data)
    {
        return -1;
    }
    return 0;
}

int network_eth_set_mtu(network_eth_device_t *dev, uint32_t mtu)
{
    if (!dev)
    {
        return -1;
    }
    dev->mtu = mtu;
    return 0;
}
