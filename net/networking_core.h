#ifndef NETWORKING_CORE_H
#define NETWORKING_CORE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    NET_IPV4,
    NET_IPV6
} NetworkProtocol;

typedef enum
{
    NET_TCP,
    NET_UDP,
    NET_ICMP
} TransportProtocol;

typedef struct
{
    uint8_t octets[4];
} IPv4Address;

typedef struct
{
    uint16_t port;
    NetworkProtocol protocol;
    TransportProtocol transport;
    IPv4Address local_ip;
    IPv4Address remote_ip;
    bool is_connected;
    uint32_t socket_id;
} Socket;

typedef struct
{
    IPv4Address ip_address;
    IPv4Address subnet_mask;
    IPv4Address gateway;
    IPv4Address dns_primary;
    IPv4Address dns_secondary;
    char mac_address[18];
    bool dhcp_enabled;
    bool is_connected;
} NetworkInterface;

typedef struct
{
    NetworkInterface interface;
    Socket sockets[256];
    uint32_t socket_count;
} NetworkCore;

NetworkCore *network_init(void);
void network_cleanup(NetworkCore *net);
int network_configure_interface(NetworkCore *net, const IPv4Address *ip, const IPv4Address *mask);
uint32_t network_socket_create(NetworkCore *net, TransportProtocol protocol, uint16_t port);
int network_socket_connect(NetworkCore *net, uint32_t socket_id, const IPv4Address *remote_ip, uint16_t port);
int network_socket_send(NetworkCore *net, uint32_t socket_id, const void *data, uint32_t size);
int network_socket_receive(NetworkCore *net, uint32_t socket_id, void *buffer, uint32_t size);
int network_socket_close(NetworkCore *net, uint32_t socket_id);

#endif
