#ifndef NET_ETHERNET_INTEL_H
#define NET_ETHERNET_INTEL_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint16_t vendor_id;
    uint16_t device_id_pci;
    uint32_t num_rx_queues;
    uint32_t num_tx_queues;
} net_eth_intel_device_t;

typedef struct
{
    uint32_t queue_id;
    uint32_t enabled;
    uint32_t size;
} net_eth_intel_queue_t;

int net_eth_intel_init(void);
int net_eth_intel_probe(void);
int net_eth_intel_open(net_eth_intel_device_t *dev);
int net_eth_intel_close(net_eth_intel_device_t *dev);
int net_eth_intel_alloc_queues(net_eth_intel_device_t *dev, uint32_t rx, uint32_t tx);
int net_eth_intel_transmit(net_eth_intel_device_t *dev, void *packet, uint32_t len);
int net_eth_intel_receive(net_eth_intel_device_t *dev, void *buffer, uint32_t len);

#endif
