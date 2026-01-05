#ifndef NET_IPV6_H
#define NET_IPV6_H

#include <stdint.h>

typedef struct
{
    uint8_t src_addr[16];
    uint8_t dst_addr[16];
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t flow_label;
} net_ipv6_route_t;

typedef struct
{
    uint32_t route_count;
    uint32_t active_routes;
    uint64_t packets_routed;
} net_ipv6_state_t;

int net_ipv6_init(void);
int net_ipv6_add_route(net_ipv6_route_t *route);
int net_ipv6_remove_route(uint8_t *dst_addr);
int net_ipv6_route_packet(uint8_t *packet, uint32_t size);
int net_ipv6_find_route(uint8_t *dst_addr, net_ipv6_route_t *route);
int net_ipv6_get_local_addr(uint32_t interface_id, uint8_t *addr);

#endif
