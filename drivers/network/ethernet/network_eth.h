#ifndef NETWORK_ETHERNET_H
#define NETWORK_ETHERNET_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint8_t mac_addr[6];
    uint32_t mtu;
    uint32_t flags;
} network_eth_device_t;

typedef struct
{
    uint32_t ipv4_addr;
    uint32_t ipv4_mask;
    uint32_t ipv4_gw;
    uint8_t ipv6_addr[16];
} network_eth_ipconfig_t;

typedef struct
{
    uint8_t *data;
    uint32_t length;
    uint32_t flags;
} network_eth_packet_t;

int network_eth_init(void);
int network_eth_configure(network_eth_device_t *dev, network_eth_ipconfig_t *cfg);
int network_eth_up(network_eth_device_t *dev);
int network_eth_down(network_eth_device_t *dev);
int network_eth_transmit(network_eth_device_t *dev, network_eth_packet_t *pkt);
int network_eth_receive(network_eth_device_t *dev, network_eth_packet_t *pkt);
int network_eth_set_mtu(network_eth_device_t *dev, uint32_t mtu);

#endif
