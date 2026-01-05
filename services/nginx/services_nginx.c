#include "services_nginx.h"

int services_nginx_init(void)
{
    return 0;
}

int services_nginx_add_server(services_nginx_server_t *server)
{
    if (!server)
    {
        return -1;
    }
    return 0;
}

int services_nginx_remove_server(uint32_t server_id)
{
    return 0;
}

int services_nginx_start_server(uint32_t server_id)
{
    return 0;
}

int services_nginx_stop_server(uint32_t server_id)
{
    return 0;
}

int services_nginx_reload_config(void)
{
    return 0;
}

int services_nginx_get_state(services_nginx_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
