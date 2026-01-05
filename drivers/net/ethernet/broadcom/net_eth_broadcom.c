#include "net_eth_broadcom.h"

int net_eth_broadcom_init(void)
{
    return 0;
}

int net_eth_broadcom_probe(void)
{
    return 0;
}

int net_eth_broadcom_open(net_eth_broadcom_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_eth_broadcom_close(net_eth_broadcom_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_eth_broadcom_transmit(net_eth_broadcom_device_t *dev, void *packet, uint32_t len)
{
    if (!dev || !packet || len == 0)
    {
        return -1;
    }
    return 0;
}

int net_eth_broadcom_get_link_status(net_eth_broadcom_device_t *dev, net_eth_broadcom_link_t *link)
{
    if (!dev || !link)
    {
        return -1;
    }
    return 0;
}
