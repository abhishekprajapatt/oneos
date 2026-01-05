#ifndef NET_ETHERNET_REALTEK_H
#define NET_ETHERNET_REALTEK_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t version;
    uint32_t irq;
    uint8_t mac_addr[6];
} net_eth_realtek_device_t;

typedef struct
{
    uint32_t rx_errors;
    uint32_t tx_errors;
    uint64_t rx_bytes;
    uint64_t tx_bytes;
} net_eth_realtek_stats_t;

int net_eth_realtek_init(void);
int net_eth_realtek_probe(void);
int net_eth_realtek_open(net_eth_realtek_device_t *dev);
int net_eth_realtek_close(net_eth_realtek_device_t *dev);
int net_eth_realtek_transmit(net_eth_realtek_device_t *dev, void *packet, uint32_t len);
int net_eth_realtek_get_stats(net_eth_realtek_device_t *dev, net_eth_realtek_stats_t *stats);

#endif
