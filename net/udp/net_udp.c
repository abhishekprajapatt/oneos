#include "net_udp.h"

int net_udp_init(void)
{
    return 0;
}

int net_udp_create_socket(net_udp_socket_t *socket)
{
    if (!socket)
    {
        return -1;
    }
    return 0;
}

int net_udp_destroy_socket(uint32_t socket_id)
{
    return 0;
}

int net_udp_bind(uint32_t socket_id, uint16_t port)
{
    return 0;
}

int net_udp_sendto(uint32_t socket_id, uint8_t *data, uint32_t size, uint8_t *addr, uint16_t port)
{
    if (!data || !addr)
    {
        return -1;
    }
    return 0;
}

int net_udp_recvfrom(uint32_t socket_id, uint8_t *data, uint32_t *size, uint8_t *src_addr, uint16_t *src_port)
{
    if (!data || !size || !src_addr || !src_port)
    {
        return -1;
    }
    return 0;
}
