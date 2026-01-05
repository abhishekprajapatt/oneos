#include "net_driver.h"

int net_init(void)
{
    return 0;
}

int net_register_device(net_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_transmit(net_device_t *dev, net_packet_t *pkt)
{
    if (!dev || !pkt || !pkt->data)
    {
        return -1;
    }
    return 0;
}

int net_receive(net_device_t *dev, net_packet_t *pkt)
{
    if (!dev || !pkt || !pkt->data)
    {
        return -1;
    }
    return 0;
}

int net_get_stats(net_device_t *dev, void *stats)
{
    if (!dev || !stats)
    {
        return -1;
    }
    return 0;
}
