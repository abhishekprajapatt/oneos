#ifndef NET_WIRELESS_ATH_H
#define NET_WIRELESS_ATH_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t hw_version;
    uint8_t mac_addr[6];
    uint32_t num_chains;
} net_wl_ath_device_t;

typedef struct
{
    uint32_t channel;
    uint32_t bandwidth;
    int8_t power;
    uint32_t flags;
} net_wl_ath_config_t;

int net_wl_ath_init(void);
int net_wl_ath_probe(void);
int net_wl_ath_open(net_wl_ath_device_t *dev);
int net_wl_ath_close(net_wl_ath_device_t *dev);
int net_wl_ath_configure(net_wl_ath_device_t *dev, net_wl_ath_config_t *cfg);
int net_wl_ath_scan(net_wl_ath_device_t *dev);

#endif
