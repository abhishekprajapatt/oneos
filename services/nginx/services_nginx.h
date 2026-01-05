#ifndef SERVICES_NGINX_H
#define SERVICES_NGINX_H

#include <stdint.h>

typedef struct
{
    uint32_t server_id;
    const char *server_name;
    uint16_t listen_port;
    const char *root_path;
    uint32_t server_state;
} services_nginx_server_t;

typedef struct
{
    uint32_t total_servers;
    uint32_t running_servers;
    uint32_t stopped_servers;
    uint64_t total_requests;
} services_nginx_state_t;

int services_nginx_init(void);
int services_nginx_add_server(services_nginx_server_t *server);
int services_nginx_remove_server(uint32_t server_id);
int services_nginx_start_server(uint32_t server_id);
int services_nginx_stop_server(uint32_t server_id);
int services_nginx_reload_config(void);
int services_nginx_get_state(services_nginx_state_t *state);

#endif
