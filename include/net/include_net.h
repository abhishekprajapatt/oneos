#ifndef INCLUDE_NET_H
#define INCLUDE_NET_H

#include <stdint.h>

typedef struct
{
    uint32_t socket_id;
    uint32_t socket_type;
    uint32_t state;
} include_net_socket_t;

typedef struct
{
    uint32_t if_id;
    const char *if_name;
    uint32_t mtu;
    uint32_t flags;
} include_net_interface_t;

int include_net_create_socket(uint32_t type, include_net_socket_t *socket);
int include_net_bind_socket(include_net_socket_t *socket, uint16_t port);
int include_net_listen_socket(include_net_socket_t *socket, uint32_t backlog);
int include_net_accept_connection(include_net_socket_t *socket, include_net_socket_t *client);
int include_net_get_interface(const char *if_name, include_net_interface_t *interface);
int include_net_set_interface_config(include_net_interface_t *interface);

#endif
