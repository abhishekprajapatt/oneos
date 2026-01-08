#include "network_stack.h"
#include <stdlib.h>
#include <string.h>

NetworkStack *net_stack_init(uint32_t mtu_size)
{
    NetworkStack *stack = (NetworkStack *)malloc(sizeof(NetworkStack));
    if (!stack)
        return NULL;

    memset(stack, 0, sizeof(NetworkStack));
    stack->mtu_size = (mtu_size > 0) ? mtu_size : 1500;
    stack->connection_count = 0;
    stack->socket_count = 0;
    stack->tcp_stream_count = 0;
    stack->route_count = 0;
    stack->arp_count = 0;
    stack->total_packets_sent = 0;
    stack->total_packets_received = 0;
    stack->total_bytes_sent = 0;
    stack->total_bytes_received = 0;
    stack->packet_loss_percent = 0;

    return stack;
}

void net_stack_cleanup(NetworkStack *stack)
{
    if (!stack)
        return;

    for (uint32_t i = 0; i < stack->tcp_stream_count; i++)
    {
        if (stack->tcp_streams[i].tx_buffer)
            free(stack->tcp_streams[i].tx_buffer);
        if (stack->tcp_streams[i].rx_buffer)
            free(stack->tcp_streams[i].rx_buffer);
    }

    memset(stack, 0, sizeof(NetworkStack));
    free(stack);
}

uint32_t net_socket_create(NetworkStack *stack, ProtocolType protocol, uint32_t local_ip, uint16_t local_port)
{
    if (!stack || stack->socket_count >= MAX_SOCKETS)
        return 0;

    Socket *socket = &stack->sockets[stack->socket_count];
    socket->socket_id = stack->socket_count + 1;
    socket->protocol = protocol;
    socket->state = CONN_STATE_CLOSED;
    socket->local_ip = local_ip;
    socket->local_port = local_port;
    socket->backlog = 0;
    socket->pending_connections = 0;

    stack->socket_count++;
    return socket->socket_id;
}

int net_socket_listen(NetworkStack *stack, uint32_t socket_id, uint32_t backlog)
{
    if (!stack)
        return -1;

    Socket *socket = net_get_socket(stack, socket_id);
    if (!socket || socket->state != CONN_STATE_CLOSED)
        return -1;

    socket->state = CONN_STATE_LISTEN;
    socket->backlog = backlog;
    socket->pending_connections = 0;
    return 0;
}

int net_socket_connect(NetworkStack *stack, uint32_t socket_id, uint32_t remote_ip, uint16_t remote_port)
{
    if (!stack)
        return -1;

    Socket *socket = net_get_socket(stack, socket_id);
    if (!socket || socket->state == CONN_STATE_ESTABLISHED)
        return -1;

    socket->state = CONN_STATE_ESTABLISHED;

    if (stack->connection_count < MAX_NETWORK_CONNECTIONS)
    {
        Connection *conn = &stack->connections[stack->connection_count];
        conn->connection_id = stack->connection_count + 1;
        conn->protocol = socket->protocol;
        conn->state = CONN_STATE_ESTABLISHED;
        conn->local_ip = socket->local_ip;
        conn->local_port = socket->local_port;
        conn->remote_ip = remote_ip;
        conn->remote_port = remote_port;
        conn->created_time = time(NULL);
        conn->last_activity = time(NULL);
        memset(&conn->stats, 0, sizeof(ConnectionStats));
        stack->connection_count++;
    }

    return 0;
}

int net_socket_close(NetworkStack *stack, uint32_t socket_id)
{
    if (!stack)
        return -1;

    Socket *socket = net_get_socket(stack, socket_id);
    if (!socket)
        return -1;

    socket->state = CONN_STATE_CLOSED;
    socket->backlog = 0;
    socket->pending_connections = 0;
    return 0;
}

int net_socket_send(NetworkStack *stack, uint32_t socket_id, const uint8_t *data, uint32_t size)
{
    if (!stack || !data || size == 0)
        return -1;

    Socket *socket = net_get_socket(stack, socket_id);
    if (!socket || socket->state != CONN_STATE_ESTABLISHED)
        return -1;

    if (size > stack->mtu_size)
        return -1;

    stack->total_packets_sent++;
    stack->total_bytes_sent += size;

    return size;
}

int net_socket_receive(NetworkStack *stack, uint32_t socket_id, uint8_t *buffer, uint32_t size)
{
    if (!stack || !buffer || size == 0)
        return -1;

    Socket *socket = net_get_socket(stack, socket_id);
    if (!socket || socket->state != CONN_STATE_ESTABLISHED)
        return -1;

    memset(buffer, 0, size);
    stack->total_packets_received++;
    stack->total_bytes_received += size;

    return 0;
}

uint32_t net_tcp_open_stream(NetworkStack *stack, uint32_t local_ip, uint16_t local_port,
                             uint32_t remote_ip, uint16_t remote_port)
{
    if (!stack || stack->tcp_stream_count >= MAX_TCP_STREAMS)
        return 0;

    TCPStream *stream = &stack->tcp_streams[stack->tcp_stream_count];
    stream->stream_id = stack->tcp_stream_count + 1;
    stream->state = TCP_STATE_SYN_SENT;
    stream->sequence_number = 1000;
    stream->acknowledgment_number = 0;
    stream->window_size = 65536;
    stream->mss = 1460;
    stream->cwnd = stream->mss;
    stream->ssthresh = 65536;
    stream->rtt_ms = 50;
    stream->retransmit_count = 0;
    stream->tx_size = 8192;
    stream->rx_size = 8192;
    stream->tx_buffer = (uint8_t *)malloc(stream->tx_size);
    stream->rx_buffer = (uint8_t *)malloc(stream->rx_size);

    if (!stream->tx_buffer || !stream->rx_buffer)
    {
        if (stream->tx_buffer)
            free(stream->tx_buffer);
        if (stream->rx_buffer)
            free(stream->rx_buffer);
        return 0;
    }

    memset(stream->tx_buffer, 0, stream->tx_size);
    memset(stream->rx_buffer, 0, stream->rx_size);
    stream->state = TCP_STATE_ESTABLISHED;

    stack->tcp_stream_count++;
    return stream->stream_id;
}

int net_tcp_close_stream(NetworkStack *stack, uint32_t stream_id)
{
    if (!stack)
        return -1;

    TCPStream *stream = net_get_tcp_stream(stack, stream_id);
    if (!stream)
        return -1;

    if (stream->tx_buffer)
        free(stream->tx_buffer);
    if (stream->rx_buffer)
        free(stream->rx_buffer);

    memmove(stream, stream + 1, (stack->tcp_stream_count - 1) * sizeof(TCPStream));
    stack->tcp_stream_count--;
    return 0;
}

int net_tcp_send_data(NetworkStack *stack, uint32_t stream_id, const uint8_t *data, uint32_t size)
{
    if (!stack || !data || size == 0)
        return -1;

    TCPStream *stream = net_get_tcp_stream(stack, stream_id);
    if (!stream || stream->state != TCP_STATE_ESTABLISHED)
        return -1;

    if (size > stream->tx_size)
        return -1;

    memcpy(stream->tx_buffer, data, size);
    stream->sequence_number += size;
    stack->total_packets_sent++;
    stack->total_bytes_sent += size;

    return size;
}

int net_tcp_receive_data(NetworkStack *stack, uint32_t stream_id, uint8_t *buffer, uint32_t size)
{
    if (!stack || !buffer || size == 0)
        return -1;

    TCPStream *stream = net_get_tcp_stream(stack, stream_id);
    if (!stream || stream->state != TCP_STATE_ESTABLISHED)
        return -1;

    memset(buffer, 0, size);
    stream->acknowledgment_number++;
    stack->total_packets_received++;
    stack->total_bytes_received += size;

    return size;
}

int net_add_route(NetworkStack *stack, uint32_t dest_ip, uint32_t gateway_ip,
                  uint32_t netmask, uint32_t metric, const char *interface)
{
    if (!stack || stack->route_count >= MAX_ROUTES || !interface)
        return -1;

    RouteEntry *route = &stack->routing_table[stack->route_count];
    route->destination_ip = dest_ip;
    route->gateway_ip = gateway_ip;
    route->netmask = netmask;
    route->metric = metric;
    strncpy(route->interface, interface, 15);
    route->interface[15] = '\0';
    route->is_active = true;

    stack->route_count++;
    return 0;
}

int net_remove_route(NetworkStack *stack, uint32_t dest_ip)
{
    if (!stack)
        return -1;

    for (uint32_t i = 0; i < stack->route_count; i++)
    {
        if (stack->routing_table[i].destination_ip == dest_ip)
        {
            memmove(&stack->routing_table[i], &stack->routing_table[i + 1],
                    (stack->route_count - i - 1) * sizeof(RouteEntry));
            stack->route_count--;
            return 0;
        }
    }
    return -1;
}

RouteEntry *net_find_route(NetworkStack *stack, uint32_t dest_ip)
{
    if (!stack)
        return NULL;

    for (uint32_t i = 0; i < stack->route_count; i++)
    {
        if ((dest_ip & stack->routing_table[i].netmask) ==
            (stack->routing_table[i].destination_ip & stack->routing_table[i].netmask))
        {
            return &stack->routing_table[i];
        }
    }
    return NULL;
}

int net_arp_add_entry(NetworkStack *stack, uint32_t ip, const uint8_t *mac_addr)
{
    if (!stack || !mac_addr || stack->arp_count >= MAX_ARP_ENTRIES)
        return -1;

    for (uint32_t i = 0; i < stack->arp_count; i++)
    {
        if (stack->arp_table[i].ip_address == ip)
        {
            memcpy(stack->arp_table[i].mac_address, mac_addr, 6);
            stack->arp_table[i].ttl_ms = 600000;
            return 0;
        }
    }

    ARPEntry *entry = &stack->arp_table[stack->arp_count];
    entry->ip_address = ip;
    memcpy(entry->mac_address, mac_addr, 6);
    entry->ttl_ms = 600000;
    entry->is_valid = true;

    stack->arp_count++;
    return 0;
}

int net_arp_resolve(NetworkStack *stack, uint32_t ip, uint8_t *mac_addr)
{
    if (!stack || !mac_addr)
        return -1;

    for (uint32_t i = 0; i < stack->arp_count; i++)
    {
        if (stack->arp_table[i].ip_address == ip && stack->arp_table[i].is_valid)
        {
            memcpy(mac_addr, stack->arp_table[i].mac_address, 6);
            return 0;
        }
    }
    return -1;
}

int net_arp_purge_expired(NetworkStack *stack)
{
    if (!stack)
        return -1;

    uint32_t new_count = 0;
    for (uint32_t i = 0; i < stack->arp_count; i++)
    {
        if (stack->arp_table[i].ttl_ms > 0)
        {
            memmove(&stack->arp_table[new_count], &stack->arp_table[i], sizeof(ARPEntry));
            stack->arp_table[new_count].ttl_ms -= 1000;
            new_count++;
        }
    }
    stack->arp_count = new_count;
    return 0;
}

Connection *net_get_connection(NetworkStack *stack, uint32_t conn_id)
{
    if (!stack)
        return NULL;

    for (uint32_t i = 0; i < stack->connection_count; i++)
    {
        if (stack->connections[i].connection_id == conn_id)
        {
            return &stack->connections[i];
        }
    }
    return NULL;
}

TCPStream *net_get_tcp_stream(NetworkStack *stack, uint32_t stream_id)
{
    if (!stack)
        return NULL;

    for (uint32_t i = 0; i < stack->tcp_stream_count; i++)
    {
        if (stack->tcp_streams[i].stream_id == stream_id)
        {
            return &stack->tcp_streams[i];
        }
    }
    return NULL;
}

Socket *net_get_socket(NetworkStack *stack, uint32_t socket_id)
{
    if (!stack)
        return NULL;

    for (uint32_t i = 0; i < stack->socket_count; i++)
    {
        if (stack->sockets[i].socket_id == socket_id)
        {
            return &stack->sockets[i];
        }
    }
    return NULL;
}

uint64_t net_get_total_packets_sent(NetworkStack *stack)
{
    if (!stack)
        return 0;
    return stack->total_packets_sent;
}

uint64_t net_get_total_packets_received(NetworkStack *stack)
{
    if (!stack)
        return 0;
    return stack->total_packets_received;
}

uint64_t net_get_total_bytes_sent(NetworkStack *stack)
{
    if (!stack)
        return 0;
    return stack->total_bytes_sent;
}

uint64_t net_get_total_bytes_received(NetworkStack *stack)
{
    if (!stack)
        return 0;
    return stack->total_bytes_received;
}
