#ifndef NET_TCP_H
#define NET_TCP_H

#include <stdint.h>

typedef struct
{
    uint32_t socket_id;
    uint8_t src_addr[4];
    uint8_t dst_addr[4];
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t state;
} net_tcp_socket_t;

typedef struct
{
    uint32_t socket_count;
    uint32_t established_count;
    uint64_t bytes_sent;
    uint64_t bytes_received;
} net_tcp_state_t;

int net_tcp_init(void);
int net_tcp_create_socket(net_tcp_socket_t *socket);
int net_tcp_destroy_socket(uint32_t socket_id);
int net_tcp_connect(uint32_t socket_id, uint8_t *addr, uint16_t port);
int net_tcp_listen(uint32_t socket_id, uint32_t backlog);
int net_tcp_accept(uint32_t socket_id, net_tcp_socket_t *client);

#endif
