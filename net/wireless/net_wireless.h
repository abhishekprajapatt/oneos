#ifndef NET_WIRELESS_H
#define NET_WIRELESS_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    const char *device_name;
    uint32_t tx_power;
    uint32_t channel;
} net_wireless_device_t;

typedef struct
{
    uint32_t device_count;
    uint32_t connected_count;
    uint64_t signal_strength;
} net_wireless_state_t;

int net_wireless_init(void);
int net_wireless_register_device(net_wireless_device_t *device);
int net_wireless_unregister_device(uint32_t device_id);
int net_wireless_scan_networks(uint32_t device_id);
int net_wireless_connect(uint32_t device_id, const char *ssid, const char *password);
int net_wireless_disconnect(uint32_t device_id);

#endif
