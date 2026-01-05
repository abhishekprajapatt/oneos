#ifndef NETWORK_WIRELESS_H
#define NETWORK_WIRELESS_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint8_t mac_addr[6];
    uint32_t freq_band;
    uint32_t max_power;
} network_wl_device_t;

typedef struct
{
    uint8_t *ssid;
    uint32_t ssid_len;
    uint32_t channel;
    int8_t signal_level;
    uint32_t security;
} network_wl_scan_result_t;

typedef struct
{
    uint32_t ipv4_addr;
    uint32_t ipv4_mask;
    uint32_t ipv4_gw;
    uint8_t dns1[4];
    uint8_t dns2[4];
} network_wl_dhcp_config_t;

int network_wl_init(void);
int network_wl_enable(network_wl_device_t *dev);
int network_wl_disable(network_wl_device_t *dev);
int network_wl_scan(network_wl_device_t *dev);
int network_wl_connect(network_wl_device_t *dev, uint8_t *ssid, uint32_t ssid_len, uint8_t *password, uint32_t pass_len);
int network_wl_disconnect(network_wl_device_t *dev);
int network_wl_set_power_save(network_wl_device_t *dev, uint32_t mode);

#endif
