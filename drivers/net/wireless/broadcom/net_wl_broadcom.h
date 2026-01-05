#ifndef NET_WIRELESS_BROADCOM_H
#define NET_WIRELESS_BROADCOM_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t chip_id;
    uint8_t mac_addr[6];
} net_wl_broadcom_device_t;

typedef struct
{
    uint32_t channel;
    uint8_t *ssid;
    uint32_t ssid_len;
    uint32_t security;
} net_wl_broadcom_scan_t;

int net_wl_broadcom_init(void);
int net_wl_broadcom_probe(void);
int net_wl_broadcom_open(net_wl_broadcom_device_t *dev);
int net_wl_broadcom_close(net_wl_broadcom_device_t *dev);
int net_wl_broadcom_scan(net_wl_broadcom_device_t *dev, net_wl_broadcom_scan_t *scan);
int net_wl_broadcom_connect(net_wl_broadcom_device_t *dev, uint8_t *ssid, uint32_t ssid_len);

#endif
