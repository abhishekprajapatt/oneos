#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>

typedef struct
{
    uint32_t interface_id;
    const char *interface_name;
    const char *ip_address;
    const char *netmask;
    const char *gateway;
    uint32_t is_connected;
    uint32_t signal_strength;
} network_interface_t;

typedef struct
{
    uint32_t total_interfaces;
    uint32_t connected_interfaces;
    uint32_t disconnected_interfaces;
    uint64_t total_data_sent;
    uint64_t total_data_received;
} network_stats_t;

int network_init(void);
int network_list_interfaces(network_interface_t *interfaces, uint32_t *count);
int network_get_interface(uint32_t interface_id, network_interface_t *info);
int network_connect(uint32_t interface_id);
int network_disconnect(uint32_t interface_id);
int network_set_ip(uint32_t interface_id, const char *ip_address);
int network_get_signal_strength(uint32_t interface_id, uint32_t *strength);
int network_get_stats(network_stats_t *stats);

#endif
