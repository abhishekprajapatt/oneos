#include "net_ipv4.h"

int net_ipv4_init(void)
{
    return 0;
}

int net_ipv4_add_route(net_ipv4_route_t *route)
{
    if (!route)
    {
        return -1;
    }
    return 0;
}

int net_ipv4_remove_route(uint8_t *dst_addr)
{
    if (!dst_addr)
    {
        return -1;
    }
    return 0;
}

int net_ipv4_route_packet(uint8_t *packet, uint32_t size)
{
    if (!packet)
    {
        return -1;
    }
    return 0;
}

int net_ipv4_find_route(uint8_t *dst_addr, net_ipv4_route_t *route)
{
    if (!dst_addr || !route)
    {
        return -1;
    }
    return 0;
}

int net_ipv4_get_local_addr(uint32_t interface_id, uint8_t *addr)
{
    if (!addr)
    {
        return -1;
    }
    return 0;
}
