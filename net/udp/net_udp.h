#ifndef NET_UDP_H
#define NET_UDP_H

#include <stdint.h>

typedef struct
{
    uint32_t socket_id;
    uint8_t src_addr[4];
    uint8_t dst_addr[4];
    uint16_t src_port;
    uint16_t dst_port;
} net_udp_socket_t;

typedef struct
{
    uint32_t socket_count;
    uint32_t active_sockets;
    uint64_t packets_sent;
    uint64_t packets_received;
} net_udp_state_t;

int net_udp_init(void);
int net_udp_create_socket(net_udp_socket_t *socket);
int net_udp_destroy_socket(uint32_t socket_id);
int net_udp_bind(uint32_t socket_id, uint16_t port);
int net_udp_sendto(uint32_t socket_id, uint8_t *data, uint32_t size, uint8_t *addr, uint16_t port);
int net_udp_recvfrom(uint32_t socket_id, uint8_t *data, uint32_t *size, uint8_t *src_addr, uint16_t *src_port);

#endif
