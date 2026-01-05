#ifndef NET_DRIVER_H
#define NET_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint8_t mac_addr[6];
    uint32_t mtu;
} net_device_t;

typedef struct
{
    uint8_t *data;
    uint32_t length;
    uint32_t flags;
} net_packet_t;

int net_init(void);
int net_register_device(net_device_t *dev);
int net_transmit(net_device_t *dev, net_packet_t *pkt);
int net_receive(net_device_t *dev, net_packet_t *pkt);
int net_get_stats(net_device_t *dev, void *stats);

#endif
