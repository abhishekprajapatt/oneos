#ifndef INIT_NETWORK_H
#define INIT_NETWORK_H

#include <stdint.h>

typedef struct
{
    uint32_t interface_id;
    const char *interface_name;
    uint8_t ip_addr[4];
    uint8_t netmask[4];
} init_network_config_t;

typedef struct
{
    uint32_t interface_count;
    uint32_t configured_count;
    uint32_t active_count;
} init_network_state_t;

int init_network_load_config(const char *config_file);
int init_network_add_interface(init_network_config_t *config);
int init_network_remove_interface(uint32_t interface_id);
int init_network_configure_interface(uint32_t interface_id);
int init_network_bring_up_interface(uint32_t interface_id);
int init_network_bring_down_interface(uint32_t interface_id);

#endif
