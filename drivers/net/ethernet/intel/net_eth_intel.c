#include "net_eth_intel.h"

int net_eth_intel_init(void)
{
    return 0;
}

int net_eth_intel_probe(void)
{
    return 0;
}

int net_eth_intel_open(net_eth_intel_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_eth_intel_close(net_eth_intel_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_eth_intel_alloc_queues(net_eth_intel_device_t *dev, uint32_t rx, uint32_t tx)
{
    if (!dev)
    {
        return -1;
    }
    dev->num_rx_queues = rx;
    dev->num_tx_queues = tx;
    return 0;
}

int net_eth_intel_transmit(net_eth_intel_device_t *dev, void *packet, uint32_t len)
{
    if (!dev || !packet || len == 0)
    {
        return -1;
    }
    return 0;
}

int net_eth_intel_receive(net_eth_intel_device_t *dev, void *buffer, uint32_t len)
{
    if (!dev || !buffer || len == 0)
    {
        return -1;
    }
    return 0;
}
