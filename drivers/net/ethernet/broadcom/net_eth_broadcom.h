#ifndef NET_ETHERNET_BROADCOM_H
#define NET_ETHERNET_BROADCOM_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint16_t vendor_id;
    uint16_t device_id_pci;
    uint32_t irq;
    uint8_t mac_addr[6];
} net_eth_broadcom_device_t;

typedef struct
{
    uint32_t speed;
    uint32_t duplex;
    uint32_t autoneg;
    uint32_t link_status;
} net_eth_broadcom_link_t;

int net_eth_broadcom_init(void);
int net_eth_broadcom_probe(void);
int net_eth_broadcom_open(net_eth_broadcom_device_t *dev);
int net_eth_broadcom_close(net_eth_broadcom_device_t *dev);
int net_eth_broadcom_transmit(net_eth_broadcom_device_t *dev, void *packet, uint32_t len);
int net_eth_broadcom_get_link_status(net_eth_broadcom_device_t *dev, net_eth_broadcom_link_t *link);

#endif
