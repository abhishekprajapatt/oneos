#ifndef SERVICES_NETWORK_H
#define SERVICES_NETWORK_H

#include <stdint.h>

typedef struct
{
    uint32_t interface_id;
    const char *interface_name;
    uint32_t interface_state;
    uint32_t ip_addr;
    uint32_t netmask;
} services_network_interface_t;

typedef struct
{
    uint32_t total_interfaces;
    uint32_t active_interfaces;
    uint32_t inactive_interfaces;
    uint64_t total_data_transferred;
} services_network_state_t;

int services_network_init(void);
int services_network_configure_interface(services_network_interface_t *iface);
int services_network_enable_interface(uint32_t interface_id);
int services_network_disable_interface(uint32_t interface_id);
int services_network_get_interface_stats(uint32_t interface_id, services_network_interface_t *iface);
int services_network_set_gateway(uint32_t gateway_addr);
int services_network_get_state(services_network_state_t *state);

#endif
