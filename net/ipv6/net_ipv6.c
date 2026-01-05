#include "net_ipv6.h"

int net_ipv6_init(void)
{
    return 0;
}

int net_ipv6_add_route(net_ipv6_route_t *route)
{
    if (!route)
    {
        return -1;
    }
    return 0;
}

int net_ipv6_remove_route(uint8_t *dst_addr)
{
    if (!dst_addr)
    {
        return -1;
    }
    return 0;
}

int net_ipv6_route_packet(uint8_t *packet, uint32_t size)
{
    if (!packet)
    {
        return -1;
    }
    return 0;
}

int net_ipv6_find_route(uint8_t *dst_addr, net_ipv6_route_t *route)
{
    if (!dst_addr || !route)
    {
        return -1;
    }
    return 0;
}

int net_ipv6_get_local_addr(uint32_t interface_id, uint8_t *addr)
{
    if (!addr)
    {
        return -1;
    }
    return 0;
}
