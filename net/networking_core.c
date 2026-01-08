#include "networking_core.h"
#include <stdlib.h>
#include <string.h>

NetworkCore *network_init(void)
{
    NetworkCore *net = (NetworkCore *)malloc(sizeof(NetworkCore));
    if (!net)
        return NULL;

    memset(net->interface.ip_address.octets, 0, 4);
    memset(net->interface.subnet_mask.octets, 0, 4);
    memset(net->interface.gateway.octets, 0, 4);
    strcpy(net->interface.mac_address, "00:00:00:00:00:00");
    net->interface.dhcp_enabled = true;
    net->interface.is_connected = false;

    net->socket_count = 0;
    memset(net->sockets, 0, sizeof(net->sockets));

    return net;
}

void network_cleanup(NetworkCore *net)
{
    if (!net)
        return;
    free(net);
}

int network_configure_interface(NetworkCore *net, const IPv4Address *ip, const IPv4Address *mask)
{
    if (!net || !ip || !mask)
        return -1;

    memcpy(&net->interface.ip_address, ip, sizeof(IPv4Address));
    memcpy(&net->interface.subnet_mask, mask, sizeof(IPv4Address));
    net->interface.is_connected = true;

    return 0;
}

uint32_t network_socket_create(NetworkCore *net, TransportProtocol protocol, uint16_t port)
{
    if (!net || net->socket_count >= 256)
        return 0;

    Socket *socket = &net->sockets[net->socket_count];
    socket->socket_id = net->socket_count + 1;
    socket->port = port;
    socket->transport = protocol;
    socket->protocol = NET_IPV4;
    socket->is_connected = false;

    net->socket_count++;
    return socket->socket_id;
}

int network_socket_connect(NetworkCore *net, uint32_t socket_id, const IPv4Address *remote_ip, uint16_t port)
{
    if (!net || !remote_ip || socket_id == 0 || socket_id > net->socket_count)
        return -1;

    Socket *socket = &net->sockets[socket_id - 1];
    memcpy(&socket->remote_ip, remote_ip, sizeof(IPv4Address));
    socket->is_connected = true;

    return 0;
}

int network_socket_send(NetworkCore *net, uint32_t socket_id, const void *data, uint32_t size)
{
    if (!net || !data || size == 0 || socket_id == 0 || socket_id > net->socket_count)
        return -1;

    Socket *socket = &net->sockets[socket_id - 1];
    if (!socket->is_connected)
        return -1;

    return size;
}

int network_socket_receive(NetworkCore *net, uint32_t socket_id, void *buffer, uint32_t size)
{
    if (!net || !buffer || size == 0 || socket_id == 0 || socket_id > net->socket_count)
        return -1;

    Socket *socket = &net->sockets[socket_id - 1];
    if (!socket->is_connected)
        return -1;

    return 0;
}

int network_socket_close(NetworkCore *net, uint32_t socket_id)
{
    if (!net || socket_id == 0 || socket_id > net->socket_count)
        return -1;

    Socket *socket = &net->sockets[socket_id - 1];
    socket->is_connected = false;

    return 0;
}
