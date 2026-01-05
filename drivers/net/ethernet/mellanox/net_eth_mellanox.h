#ifndef NET_ETHERNET_MELLANOX_H
#define NET_ETHERNET_MELLANOX_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t fw_version;
    uint32_t num_ports;
    uint32_t num_pf;
} net_eth_mellanox_device_t;

typedef struct
{
    uint32_t speed;
    uint32_t width;
    uint32_t mtu;
} net_eth_mellanox_params_t;

int net_eth_mellanox_init(void);
int net_eth_mellanox_probe(void);
int net_eth_mellanox_open(net_eth_mellanox_device_t *dev);
int net_eth_mellanox_close(net_eth_mellanox_device_t *dev);
int net_eth_mellanox_set_params(net_eth_mellanox_device_t *dev, net_eth_mellanox_params_t *params);
int net_eth_mellanox_query_status(net_eth_mellanox_device_t *dev, void *status);

#endif
