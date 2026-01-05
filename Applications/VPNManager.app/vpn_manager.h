#ifndef VPN_MANAGER_H
#define VPN_MANAGER_H

#include <stdint.h>

typedef struct
{
    uint32_t profile_id;
    const char *profile_name;
    const char *server_address;
    uint32_t server_port;
    const char *protocol;
    uint32_t is_connected;
} vpn_profile_t;

typedef struct
{
    uint32_t total_profiles;
    uint32_t connected_profiles;
    uint64_t bytes_sent;
    uint64_t bytes_received;
} vpn_stats_t;

int vpn_manager_init(void);
int vpn_manager_create_profile(vpn_profile_t *profile);
int vpn_manager_delete_profile(uint32_t profile_id);
int vpn_manager_connect(uint32_t profile_id);
int vpn_manager_disconnect(uint32_t profile_id);
int vpn_manager_list_profiles(vpn_profile_t *profiles, uint32_t *count);
int vpn_manager_get_profile(uint32_t profile_id, vpn_profile_t *profile);
int vpn_manager_get_stats(vpn_stats_t *stats);

#endif
