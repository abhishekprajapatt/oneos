#ifndef NET_ETHERNET_MARVELL_H
#define NET_ETHERNET_MARVELL_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint16_t vendor_id;
    uint8_t mac_addr[6];
    uint32_t phy_addr;
} net_eth_marvell_device_t;

typedef struct
{
    uint32_t rate;
    uint32_t duplex;
    uint32_t flow_ctrl;
} net_eth_marvell_config_t;

int net_eth_marvell_init(void);
int net_eth_marvell_probe(void);
int net_eth_marvell_open(net_eth_marvell_device_t *dev);
int net_eth_marvell_close(net_eth_marvell_device_t *dev);
int net_eth_marvell_configure(net_eth_marvell_device_t *dev, net_eth_marvell_config_t *cfg);
int net_eth_marvell_transmit(net_eth_marvell_device_t *dev, void *packet, uint32_t len);

#endif
