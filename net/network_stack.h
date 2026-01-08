#ifndef NETWORK_STACK_H
#define NETWORK_STACK_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_NETWORK_CONNECTIONS 2048
#define MAX_SOCKETS 4096
#define MAX_ROUTES 512
#define MAX_ARP_ENTRIES 256
#define MAX_TCP_STREAMS 1024
#define MAX_UDP_SESSIONS 512

typedef enum
{
    PROTO_TCP = 0,
    PROTO_UDP = 1,
    PROTO_ICMP = 2,
    PROTO_IGMP = 3
} ProtocolType;

typedef enum
{
    CONN_STATE_CLOSED = 0,
    CONN_STATE_LISTEN = 1,
    CONN_STATE_ESTABLISHED = 2,
    CONN_STATE_CLOSING = 3,
    CONN_STATE_TIME_WAIT = 4
} ConnectionState;

typedef enum
{
    TCP_STATE_CLOSED = 0,
    TCP_STATE_LISTEN = 1,
    TCP_STATE_SYN_SENT = 2,
    TCP_STATE_SYN_RECV = 3,
    TCP_STATE_ESTABLISHED = 4,
    TCP_STATE_FIN_WAIT_1 = 5,
    TCP_STATE_FIN_WAIT_2 = 6,
    TCP_STATE_CLOSING = 7,
    TCP_STATE_TIME_WAIT = 8
} TCPState;

typedef struct
{
    uint32_t ip_address;
    uint16_t port;
    uint32_t bytes_sent;
    uint32_t bytes_received;
    uint32_t packets_sent;
    uint32_t packets_received;
    uint32_t errors;
} ConnectionStats;

typedef struct
{
    uint32_t connection_id;
    ProtocolType protocol;
    ConnectionState state;
    uint32_t local_ip;
    uint32_t remote_ip;
    uint16_t local_port;
    uint16_t remote_port;
    ConnectionStats stats;
    time_t created_time;
    time_t last_activity;
} Connection;

typedef struct
{
    uint32_t stream_id;
    TCPState state;
    uint32_t sequence_number;
    uint32_t acknowledgment_number;
    uint32_t window_size;
    uint32_t mss;
    uint32_t cwnd;
    uint32_t ssthresh;
    uint32_t rtt_ms;
    uint32_t retransmit_count;
    uint8_t *tx_buffer;
    uint8_t *rx_buffer;
    uint32_t tx_size;
    uint32_t rx_size;
} TCPStream;

typedef struct
{
    uint32_t socket_id;
    ProtocolType protocol;
    ConnectionState state;
    uint32_t local_ip;
    uint16_t local_port;
    uint32_t backlog;
    uint32_t pending_connections;
} Socket;

typedef struct
{
    uint32_t destination_ip;
    uint32_t gateway_ip;
    uint32_t netmask;
    uint32_t metric;
    char interface[16];
    bool is_active;
} RouteEntry;

typedef struct
{
    uint32_t ip_address;
    uint8_t mac_address[6];
    uint32_t ttl_ms;
    bool is_valid;
} ARPEntry;

typedef struct
{
    Connection connections[MAX_NETWORK_CONNECTIONS];
    uint32_t connection_count;
    Socket sockets[MAX_SOCKETS];
    uint32_t socket_count;
    TCPStream tcp_streams[MAX_TCP_STREAMS];
    uint32_t tcp_stream_count;
    RouteEntry routing_table[MAX_ROUTES];
    uint32_t route_count;
    ARPEntry arp_table[MAX_ARP_ENTRIES];
    uint32_t arp_count;
    uint32_t mtu_size;
    uint64_t total_packets_sent;
    uint64_t total_packets_received;
    uint64_t total_bytes_sent;
    uint64_t total_bytes_received;
    uint32_t packet_loss_percent;
} NetworkStack;

NetworkStack *net_stack_init(uint32_t mtu_size);
void net_stack_cleanup(NetworkStack *stack);

uint32_t net_socket_create(NetworkStack *stack, ProtocolType protocol, uint32_t local_ip, uint16_t local_port);
int net_socket_listen(NetworkStack *stack, uint32_t socket_id, uint32_t backlog);
int net_socket_connect(NetworkStack *stack, uint32_t socket_id, uint32_t remote_ip, uint16_t remote_port);
int net_socket_close(NetworkStack *stack, uint32_t socket_id);
int net_socket_send(NetworkStack *stack, uint32_t socket_id, const uint8_t *data, uint32_t size);
int net_socket_receive(NetworkStack *stack, uint32_t socket_id, uint8_t *buffer, uint32_t size);

uint32_t net_tcp_open_stream(NetworkStack *stack, uint32_t local_ip, uint16_t local_port,
                             uint32_t remote_ip, uint16_t remote_port);
int net_tcp_close_stream(NetworkStack *stack, uint32_t stream_id);
int net_tcp_send_data(NetworkStack *stack, uint32_t stream_id, const uint8_t *data, uint32_t size);
int net_tcp_receive_data(NetworkStack *stack, uint32_t stream_id, uint8_t *buffer, uint32_t size);

int net_add_route(NetworkStack *stack, uint32_t dest_ip, uint32_t gateway_ip,
                  uint32_t netmask, uint32_t metric, const char *interface);
int net_remove_route(NetworkStack *stack, uint32_t dest_ip);
RouteEntry *net_find_route(NetworkStack *stack, uint32_t dest_ip);

int net_arp_add_entry(NetworkStack *stack, uint32_t ip, const uint8_t *mac_addr);
int net_arp_resolve(NetworkStack *stack, uint32_t ip, uint8_t *mac_addr);
int net_arp_purge_expired(NetworkStack *stack);

Connection *net_get_connection(NetworkStack *stack, uint32_t conn_id);
TCPStream *net_get_tcp_stream(NetworkStack *stack, uint32_t stream_id);
Socket *net_get_socket(NetworkStack *stack, uint32_t socket_id);

uint64_t net_get_total_packets_sent(NetworkStack *stack);
uint64_t net_get_total_packets_received(NetworkStack *stack);
uint64_t net_get_total_bytes_sent(NetworkStack *stack);
uint64_t net_get_total_bytes_received(NetworkStack *stack);

#endif
