#include "net_tcp.h"

int net_tcp_init(void)
{
    return 0;
}

int net_tcp_create_socket(net_tcp_socket_t *socket)
{
    if (!socket)
    {
        return -1;
    }
    return 0;
}

int net_tcp_destroy_socket(uint32_t socket_id)
{
    return 0;
}

int net_tcp_connect(uint32_t socket_id, uint8_t *addr, uint16_t port)
{
    if (!addr)
    {
        return -1;
    }
    return 0;
}

int net_tcp_listen(uint32_t socket_id, uint32_t backlog)
{
    return 0;
}

int net_tcp_accept(uint32_t socket_id, net_tcp_socket_t *client)
{
    if (!client)
    {
        return -1;
    }
    return 0;
}
