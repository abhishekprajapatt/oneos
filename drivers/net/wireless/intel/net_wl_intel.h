#ifndef NET_WIRELESS_INTEL_H
#define NET_WIRELESS_INTEL_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t fw_version;
    uint8_t mac_addr[6];
    uint32_t band_support;
} net_wl_intel_device_t;

typedef struct
{
    uint32_t rate;
    int8_t power;
    uint32_t antenna_mask;
} net_wl_intel_tx_params_t;

int net_wl_intel_init(void);
int net_wl_intel_probe(void);
int net_wl_intel_open(net_wl_intel_device_t *dev);
int net_wl_intel_close(net_wl_intel_device_t *dev);
int net_wl_intel_transmit(net_wl_intel_device_t *dev, void *packet, net_wl_intel_tx_params_t *params);
int net_wl_intel_set_power(net_wl_intel_device_t *dev, uint32_t power_mode);

#endif
