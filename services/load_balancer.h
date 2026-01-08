#ifndef LOAD_BALANCER_H
#define LOAD_BALANCER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_BACKEND_SERVERS 128
#define MAX_LOAD_BALANCERS 16
#define MAX_CONNECTIONS_PER_SERVER 4096
#define MAX_SESSION_DATA 512

typedef enum
{
    ALGO_ROUND_ROBIN = 0,
    ALGO_LEAST_CONNECTIONS = 1,
    ALGO_WEIGHTED_ROUND_ROBIN = 2,
    ALGO_IP_HASH = 3,
    ALGO_LEAST_RESPONSE_TIME = 4
} LoadBalancingAlgorithm;

typedef enum
{
    HEALTH_CHECK_HTTP = 0,
    HEALTH_CHECK_TCP = 1,
    HEALTH_CHECK_ICMP = 2,
    HEALTH_CHECK_CUSTOM = 3
} HealthCheckType;

typedef enum
{
    SERVER_ACTIVE = 0,
    SERVER_INACTIVE = 1,
    SERVER_DRAINING = 2,
    SERVER_DOWN = 3
} ServerState;

typedef struct
{
    uint32_t server_id;
    uint32_t ip_address;
    uint16_t port;
    uint16_t weight;
    ServerState state;
    uint32_t current_connections;
    uint32_t max_connections;
    uint64_t total_requests;
    uint64_t total_bytes_in;
    uint64_t total_bytes_out;
    uint32_t avg_response_time_ms;
    uint64_t last_health_check;
    uint8_t consecutive_failures;
} BackendServer;

typedef struct
{
    uint32_t lb_id;
    char name[64];
    LoadBalancingAlgorithm algorithm;
    uint32_t server_count;
    BackendServer servers[MAX_BACKEND_SERVERS];
    uint32_t current_server_idx;
    uint16_t listen_port;
    uint32_t ip_address;
    HealthCheckType health_check_type;
    uint32_t health_check_interval;
    uint32_t health_check_timeout;
    uint64_t total_requests;
    uint64_t total_connections;
    uint64_t bytes_processed;
} LoadBalancer;

typedef struct
{
    uint64_t connection_id;
    uint32_t client_ip;
    uint32_t server_id;
    uint32_t server_ip;
    uint16_t server_port;
    uint64_t bytes_sent;
    uint64_t bytes_received;
    uint64_t connection_start_time;
    uint32_t response_time_ms;
    uint8_t session_data[MAX_SESSION_DATA];
    uint32_t session_data_size;
} ClientConnection;

int lb_create(const char *name, LoadBalancingAlgorithm algo, uint32_t listen_port);
int lb_add_backend_server(uint32_t lb_id, uint32_t ip, uint16_t port, uint16_t weight);
int lb_remove_backend_server(uint32_t lb_id, uint32_t server_id);
int lb_set_server_weight(uint32_t lb_id, uint32_t server_id, uint16_t weight);

int lb_mark_server_draining(uint32_t lb_id, uint32_t server_id);
int lb_mark_server_down(uint32_t lb_id, uint32_t server_id);
int lb_restore_server(uint32_t lb_id, uint32_t server_id);
ServerState lb_get_server_state(uint32_t lb_id, uint32_t server_id);

int lb_set_health_check(uint32_t lb_id, HealthCheckType type, uint32_t interval, uint32_t timeout);
int lb_perform_health_check(uint32_t lb_id, uint32_t server_id);
int lb_health_check_all_servers(uint32_t lb_id);

uint32_t lb_select_backend_server(uint32_t lb_id, uint32_t client_ip, const void *request_data, size_t request_size);
int lb_route_request(uint32_t lb_id, uint32_t client_ip, const void *request_data, size_t request_size);
int lb_handle_client_connection(uint32_t lb_id, uint64_t conn_id, uint32_t client_ip);
int lb_close_client_connection(uint32_t lb_id, uint64_t conn_id);

int lb_store_session_data(uint32_t lb_id, uint64_t conn_id, const void *data, size_t size);
void *lb_retrieve_session_data(uint32_t lb_id, uint64_t conn_id, size_t *size);

int lb_update_connection_stats(uint32_t lb_id, uint32_t server_id, uint64_t bytes_in, uint64_t bytes_out, uint32_t response_time);
uint32_t lb_get_active_connections(uint32_t lb_id, uint32_t server_id);
uint32_t lb_get_total_connections_lb(uint32_t lb_id);

int lb_rebalance_load(uint32_t lb_id);
float lb_get_server_utilization(uint32_t lb_id, uint32_t server_id);
int lb_get_least_loaded_server(uint32_t lb_id);

LoadBalancer *lb_get_info(uint32_t lb_id);
uint64_t lb_get_total_requests(uint32_t lb_id);
uint64_t lb_get_bytes_processed(uint32_t lb_id);

#endif 
