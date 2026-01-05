#include "include_net.h"

int include_net_create_socket(uint32_t type, include_net_socket_t *socket)
{
    if (!socket)
    {
        return -1;
    }
    return 0;
}

int include_net_bind_socket(include_net_socket_t *socket, uint16_t port)
{
    if (!socket)
    {
        return -1;
    }
    return 0;
}

int include_net_listen_socket(include_net_socket_t *socket, uint32_t backlog)
{
    if (!socket)
    {
        return -1;
    }
    return 0;
}

int include_net_accept_connection(include_net_socket_t *socket, include_net_socket_t *client)
{
    if (!socket || !client)
    {
        return -1;
    }
    return 0;
}

int include_net_get_interface(const char *if_name, include_net_interface_t *interface)
{
    if (!if_name || !interface)
    {
        return -1;
    }
    return 0;
}

int include_net_set_interface_config(include_net_interface_t *interface)
{
    if (!interface)
    {
        return -1;
    }
    return 0;
}
