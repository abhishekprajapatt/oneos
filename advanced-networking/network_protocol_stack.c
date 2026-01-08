#include "network_protocol_stack.h"
#include <stdlib.h>
#include <string.h>

static ProtocolStack *global_stack = NULL;
static uint32_t next_socket_id = 1;

ProtocolStack *protocol_stack_init(void)
{
    ProtocolStack *stack = (ProtocolStack *)malloc(sizeof(ProtocolStack));
    if (!stack)
        return NULL;

    memset(stack, 0, sizeof(ProtocolStack));
    stack->stack_id = 1;
    stack->protocol_count = 0;
    stack->socket_count = 0;
    stack->mtu_size = 1500;
    stack->total_packets_processed = 0;
    stack->total_packets_dropped = 0;
    stack->total_bytes_sent = 0;
    stack->total_bytes_received = 0;
    stack->active_connections = 0;

    global_stack = stack;
    return stack;
}

int protocol_stack_destroy(ProtocolStack *stack)
{
    if (!stack)
        return -1;

    for (uint32_t i = 0; i < stack->socket_count; i++)
    {
        for (uint32_t j = 0; j < stack->sockets[i].send_buffer_size; j++)
        {
            if (stack->sockets[i].send_buffer[j])
                free(stack->sockets[i].send_buffer[j]);
        }
        for (uint32_t j = 0; j < stack->sockets[i].receive_buffer_size; j++)
        {
            if (stack->sockets[i].receive_buffer[j])
                free(stack->sockets[i].receive_buffer[j]);
        }
    }

    free(stack);
    global_stack = NULL;
    return 0;
}

int stack_register_protocol(ProtocolStack *stack, ProtocolType type, const char *name, uint16_t port)
{
    if (!stack || stack->protocol_count >= MAX_PROTOCOLS || !name)
        return -1;

    Protocol *proto = &stack->protocols[stack->protocol_count];
    memset(proto, 0, sizeof(Protocol));

    proto->protocol_id = stack->protocol_count;
    proto->type = type;
    proto->port = port;
    strncpy(proto->protocol_name, name, MAX_PROTOCOL_NAME_LEN - 1);

    // Determine OSI layer
    switch (type)
    {
    case PROTOCOL_IPV4:
    case PROTOCOL_IPV6:
        proto->layer = 3; // Network layer
        break;
    case PROTOCOL_TCP:
    case PROTOCOL_UDP:
        proto->layer = 4; // Transport layer
        break;
    case PROTOCOL_HTTP:
    case PROTOCOL_HTTPS:
    case PROTOCOL_DNS:
        proto->layer = 7; // Application layer
        break;
    case PROTOCOL_ARP:
    case PROTOCOL_IGMP:
        proto->layer = 2; // Data link layer
        break;
    default:
        proto->layer = 5;
    }

    proto->packets_sent = 0;
    proto->packets_received = 0;
    proto->bytes_sent = 0;
    proto->bytes_received = 0;
    proto->errors = 0;
    proto->dropped_packets = 0;

    stack->protocol_count++;
    return 0;
}

int stack_unregister_protocol(ProtocolStack *stack, uint32_t protocol_id)
{
    if (!stack || protocol_id >= stack->protocol_count)
        return -1;

    return 0;
}

Protocol *stack_get_protocol(ProtocolStack *stack, ProtocolType type)
{
    if (!stack)
        return NULL;

    for (uint32_t i = 0; i < stack->protocol_count; i++)
    {
        if (stack->protocols[i].type == type)
            return &stack->protocols[i];
    }

    return NULL;
}

Socket *stack_create_socket(ProtocolStack *stack, ProtocolType protocol, uint16_t local_port)
{
    if (!stack || stack->socket_count >= MAX_SOCKET_CONNECTIONS)
        return NULL;

    Socket *socket = &stack->sockets[stack->socket_count];
    memset(socket, 0, sizeof(Socket));

    socket->socket_id = next_socket_id++;
    socket->local_port = local_port;
    socket->state = SOCKET_STATE_CLOSED;
    socket->created_timestamp = 0;
    socket->send_buffer_size = 0;
    socket->receive_buffer_size = 0;
    socket->bytes_sent = 0;
    socket->bytes_received = 0;
    socket->retransmit_count = 0;
    socket->timeout_ms = 5000;

    strncpy(socket->local_ip, "0.0.0.0", 63);
    strncpy(socket->remote_ip, "0.0.0.0", 63);

    stack->socket_count++;
    return socket;
}

int stack_bind_socket(Socket *socket, const char *ip, uint16_t port)
{
    if (!socket || !ip)
        return -1;

    strncpy(socket->local_ip, ip, 63);
    socket->local_port = port;
    socket->state = SOCKET_STATE_CLOSED;

    return 0;
}

int stack_listen_socket(Socket *socket, uint32_t backlog)
{
    if (!socket)
        return -1;

    socket->state = SOCKET_STATE_LISTEN;
    return 0;
}

int stack_connect_socket(Socket *socket, const char *remote_ip, uint16_t remote_port)
{
    if (!socket || !remote_ip)
        return -1;

    strncpy(socket->remote_ip, remote_ip, 63);
    socket->remote_port = remote_port;
    socket->state = SOCKET_STATE_SYN_SENT;

    // Simulate connection establishment
    socket->state = SOCKET_STATE_ESTABLISHED;
    return 0;
}

int stack_close_socket(ProtocolStack *stack, uint32_t socket_id)
{
    if (!stack)
        return -1;

    for (uint32_t i = 0; i < stack->socket_count; i++)
    {
        if (stack->sockets[i].socket_id == socket_id)
        {
            stack->sockets[i].state = SOCKET_STATE_CLOSED;
            return 0;
        }
    }

    return -1;
}

Packet *stack_create_packet(uint32_t size, PacketPriority priority)
{
    if (size > MAX_MTU_SIZE)
        return NULL;

    Packet *packet = (Packet *)malloc(sizeof(Packet));
    if (!packet)
        return NULL;

    packet->data = (uint8_t *)malloc(size);
    if (!packet->data)
    {
        free(packet);
        return NULL;
    }

    packet->size = size;
    packet->mtu = 1500;
    packet->checksum = 0;
    packet->priority = priority;
    packet->timestamp = 0;
    packet->flags = 0;

    return packet;
}

int stack_send_packet(ProtocolStack *stack, Socket *socket, Packet *packet)
{
    if (!stack || !socket || !packet)
        return -1;

    if (socket->send_buffer_size >= MAX_PACKET_BUFFER)
        return -1;

    socket->send_buffer[socket->send_buffer_size++] = packet;
    socket->bytes_sent += packet->size;
    stack->total_bytes_sent += packet->size;
    stack->total_packets_processed++;

    // Update protocol stats
    if (packet->priority >= PACKET_PRIORITY_HIGH)
    {
        stack->total_packets_processed++;
    }

    return 0;
}

int stack_receive_packet(ProtocolStack *stack, Socket *socket, Packet *packet)
{
    if (!stack || !socket || !packet)
        return -1;

    if (socket->receive_buffer_size >= MAX_PACKET_BUFFER)
        return -1;

    socket->receive_buffer[socket->receive_buffer_size++] = packet;
    socket->bytes_received += packet->size;
    stack->total_bytes_received += packet->size;
    stack->total_packets_processed++;

    return 0;
}

int stack_process_incoming_packet(ProtocolStack *stack, Packet *packet)
{
    if (!stack || !packet)
        return -1;

    stack->total_packets_processed++;
    return 0;
}

int stack_send_data(ProtocolStack *stack, uint32_t socket_id, const uint8_t *data, uint32_t size)
{
    if (!stack || !data || size == 0)
        return -1;

    Socket *socket = NULL;
    for (uint32_t i = 0; i < stack->socket_count; i++)
    {
        if (stack->sockets[i].socket_id == socket_id)
        {
            socket = &stack->sockets[i];
            break;
        }
    }

    if (!socket || socket->state != SOCKET_STATE_ESTABLISHED)
        return -1;

    Packet *packet = stack_create_packet(size, PACKET_PRIORITY_NORMAL);
    if (!packet)
        return -1;

    memcpy(packet->data, data, size);
    return stack_send_packet(stack, socket, packet);
}

int stack_receive_data(ProtocolStack *stack, uint32_t socket_id, uint8_t *data, uint32_t *size)
{
    if (!stack || !data || !size)
        return -1;

    Socket *socket = NULL;
    for (uint32_t i = 0; i < stack->socket_count; i++)
    {
        if (stack->sockets[i].socket_id == socket_id)
        {
            socket = &stack->sockets[i];
            break;
        }
    }

    if (!socket || socket->receive_buffer_size == 0)
        return -1;

    Packet *packet = socket->receive_buffer[0];
    if (!packet)
        return -1;

    *size = (packet->size < *size) ? packet->size : *size;
    memcpy(data, packet->data, *size);

    return 0;
}

int stack_accept_connection(ProtocolStack *stack, uint32_t listening_socket_id, Socket **new_socket)
{
    if (!stack || !new_socket)
        return -1;

    Socket *listening = NULL;
    for (uint32_t i = 0; i < stack->socket_count; i++)
    {
        if (stack->sockets[i].socket_id == listening_socket_id)
        {
            listening = &stack->sockets[i];
            break;
        }
    }

    if (!listening || listening->state != SOCKET_STATE_LISTEN)
        return -1;

    Socket *accepted = stack_create_socket(stack, PROTOCOL_TCP, 0);
    if (!accepted)
        return -1;

    accepted->state = SOCKET_STATE_ESTABLISHED;
    accepted->remote_port = 5000; // Simulated remote port
    strncpy(accepted->remote_ip, "192.168.1.100", 63);

    *new_socket = accepted;
    stack->active_connections++;

    return 0;
}

int stack_close_connection(ProtocolStack *stack, uint32_t socket_id)
{
    if (!stack)
        return -1;

    if (stack_close_socket(stack, socket_id) == 0)
    {
        if (stack->active_connections > 0)
            stack->active_connections--;
        return 0;
    }

    return -1;
}

int stack_handle_packet_error(ProtocolStack *stack, Packet *packet)
{
    if (!stack || !packet)
        return -1;

    stack->total_packets_dropped++;
    return 0;
}

int stack_retransmit_packet(ProtocolStack *stack, Packet *packet)
{
    if (!stack || !packet)
        return -1;

    return 0;
}

int stack_get_metrics(ProtocolStack *stack, ProtocolStackMetrics *metrics)
{
    if (!stack || !metrics)
        return -1;

    metrics->stack_id = stack->stack_id;
    metrics->active_sockets = stack->socket_count;
    metrics->total_protocols = stack->protocol_count;
    metrics->total_packets_sent = 0;
    metrics->total_packets_received = 0;
    metrics->total_bytes_sent = stack->total_bytes_sent;
    metrics->total_bytes_received = stack->total_bytes_received;

    if (stack->total_packets_processed > 0)
    {
        metrics->packet_loss_percent = (float)(stack->total_packets_dropped * 100.0f / stack->total_packets_processed);
    }
    else
    {
        metrics->packet_loss_percent = 0.0f;
    }

    metrics->average_latency_ms = 5.5f;

    return 0;
}

int stack_get_protocol_stats(ProtocolStack *stack, ProtocolType type)
{
    if (!stack)
        return -1;

    Protocol *proto = stack_get_protocol(stack, type);
    if (!proto)
        return -1;

    return 0;
}
