#ifndef NET_WIRELESS_REALTEK_H
#define NET_WIRELESS_REALTEK_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t hw_type;
    uint8_t mac_addr[6];
} net_wl_realtek_device_t;

typedef struct
{
    uint32_t channel;
    uint8_t *bssid;
    uint8_t *ssid;
    uint32_t ssid_len;
} net_wl_realtek_assoc_t;

int net_wl_realtek_init(void);
int net_wl_realtek_probe(void);
int net_wl_realtek_open(net_wl_realtek_device_t *dev);
int net_wl_realtek_close(net_wl_realtek_device_t *dev);
int net_wl_realtek_associate(net_wl_realtek_device_t *dev, net_wl_realtek_assoc_t *assoc);
int net_wl_realtek_disassociate(net_wl_realtek_device_t *dev);

#endif
