#ifndef NETWORK_PROTOCOL_STACK_H
#define NETWORK_PROTOCOL_STACK_H

#include <stdint.h>
#include <stddef.h>

#define MAX_PROTOCOLS 32
#define MAX_PROTOCOL_NAME_LEN 32
#define MAX_MTU_SIZE 65536
#define MAX_PACKET_BUFFER 1024
#define MAX_SOCKET_CONNECTIONS 4096

typedef enum
{
    PROTOCOL_IPV4,
    PROTOCOL_IPV6,
    PROTOCOL_TCP,
    PROTOCOL_UDP,
    PROTOCOL_ICMP,
    PROTOCOL_HTTP,
    PROTOCOL_HTTPS,
    PROTOCOL_DNS,
    PROTOCOL_DHCP,
    PROTOCOL_ARP,
    PROTOCOL_IGMP,
    PROTOCOL_CUSTOM
} ProtocolType;

typedef enum
{
    SOCKET_STATE_CLOSED,
    SOCKET_STATE_LISTEN,
    SOCKET_STATE_SYN_SENT,
    SOCKET_STATE_SYN_RECEIVED,
    SOCKET_STATE_ESTABLISHED,
    SOCKET_STATE_CLOSE_WAIT,
    SOCKET_STATE_CLOSING,
    SOCKET_STATE_TIME_WAIT
} SocketState;

typedef enum
{
    PACKET_PRIORITY_LOW,
    PACKET_PRIORITY_NORMAL,
    PACKET_PRIORITY_HIGH,
    PACKET_PRIORITY_CRITICAL
} PacketPriority;

typedef struct
{
    uint8_t *data;
    uint32_t size;
    uint32_t mtu;
    uint32_t checksum;
    PacketPriority priority;
    uint32_t timestamp;
    uint8_t flags;
} Packet;

typedef struct
{
    uint32_t protocol_id;
    char protocol_name[MAX_PROTOCOL_NAME_LEN];
    ProtocolType type;

    uint16_t layer; // OSI layer (2-7)
    uint16_t port;

    uint32_t packets_sent;
    uint32_t packets_received;
    uint32_t bytes_sent;
    uint32_t bytes_received;

    uint32_t errors;
    uint32_t dropped_packets;

    int (*init)(void);
    int (*process_packet)(Packet *pkt);
    int (*send_packet)(Packet *pkt);
    int (*cleanup)(void);
} Protocol;

typedef struct
{
    uint32_t socket_id;
    uint16_t local_port;
    uint16_t remote_port;

    char local_ip[64];
    char remote_ip[64];

    SocketState state;
    uint32_t created_timestamp;

    Packet *send_buffer[MAX_PACKET_BUFFER];
    uint32_t send_buffer_size;

    Packet *receive_buffer[MAX_PACKET_BUFFER];
    uint32_t receive_buffer_size;

    uint32_t bytes_sent;
    uint32_t bytes_received;

    uint32_t retransmit_count;
    uint32_t timeout_ms;
} Socket;

typedef struct
{
    uint32_t stack_id;

    uint32_t protocol_count;
    Protocol protocols[MAX_PROTOCOLS];

    uint32_t socket_count;
    Socket sockets[MAX_SOCKET_CONNECTIONS];

    uint32_t mtu_size;
    uint32_t total_packets_processed;
    uint32_t total_packets_dropped;

    uint64_t total_bytes_sent;
    uint64_t total_bytes_received;

    uint32_t active_connections;
} ProtocolStack;

// Stack initialization
ProtocolStack *protocol_stack_init(void);
int protocol_stack_destroy(ProtocolStack *stack);

// Protocol management
int stack_register_protocol(ProtocolStack *stack, ProtocolType type, const char *name, uint16_t port);
int stack_unregister_protocol(ProtocolStack *stack, uint32_t protocol_id);
Protocol *stack_get_protocol(ProtocolStack *stack, ProtocolType type);

// Socket operations
Socket *stack_create_socket(ProtocolStack *stack, ProtocolType protocol, uint16_t local_port);
int stack_bind_socket(Socket *socket, const char *ip, uint16_t port);
int stack_listen_socket(Socket *socket, uint32_t backlog);
int stack_connect_socket(Socket *socket, const char *remote_ip, uint16_t remote_port);
int stack_close_socket(ProtocolStack *stack, uint32_t socket_id);

// Packet operations
Packet *stack_create_packet(uint32_t size, PacketPriority priority);
int stack_send_packet(ProtocolStack *stack, Socket *socket, Packet *packet);
int stack_receive_packet(ProtocolStack *stack, Socket *socket, Packet *packet);
int stack_process_incoming_packet(ProtocolStack *stack, Packet *packet);

// Data transmission
int stack_send_data(ProtocolStack *stack, uint32_t socket_id, const uint8_t *data, uint32_t size);
int stack_receive_data(ProtocolStack *stack, uint32_t socket_id, uint8_t *data, uint32_t *size);

// Connection management
int stack_accept_connection(ProtocolStack *stack, uint32_t listening_socket_id, Socket **new_socket);
int stack_close_connection(ProtocolStack *stack, uint32_t socket_id);

// Error handling
int stack_handle_packet_error(ProtocolStack *stack, Packet *packet);
int stack_retransmit_packet(ProtocolStack *stack, Packet *packet);

// Monitoring
typedef struct
{
    uint32_t stack_id;
    uint32_t active_sockets;
    uint32_t total_protocols;
    uint64_t total_packets_sent;
    uint64_t total_packets_received;
    uint64_t total_bytes_sent;
    uint64_t total_bytes_received;
    float packet_loss_percent;
    float average_latency_ms;
} ProtocolStackMetrics;

int stack_get_metrics(ProtocolStack *stack, ProtocolStackMetrics *metrics);
int stack_get_protocol_stats(ProtocolStack *stack, ProtocolType type);

#endif // NETWORK_PROTOCOL_STACK_H
