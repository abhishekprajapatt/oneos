#ifndef LOAD_BALANCER_H
#define LOAD_BALANCER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_BACKEND_SERVERS 256
#define MAX_LOAD_BALANCER_POOLS 64
#define MAX_PENDING_REQUESTS 4096
#define MAX_CONNECTION_TRACKING 8192

typedef enum
{
    LB_ALGORITHM_ROUND_ROBIN,
    LB_ALGORITHM_LEAST_CONN,
    LB_ALGORITHM_WEIGHTED,
    LB_ALGORITHM_IP_HASH,
    LB_ALGORITHM_CONSISTENT_HASH,
    LB_ALGORITHM_RANDOM
} LoadBalancingAlgorithm;

typedef enum
{
    SERVER_STATE_HEALTHY,
    SERVER_STATE_DEGRADED,
    SERVER_STATE_UNHEALTHY,
    SERVER_STATE_DRAINING,
    SERVER_STATE_MAINTENANCE
} ServerState;

typedef struct
{
    uint32_t server_id;
    uint32_t server_ip;
    uint16_t server_port;

    ServerState state;
    uint32_t weight;

    uint32_t active_connections;
    uint32_t total_requests;
    uint64_t bytes_sent;
    uint64_t bytes_received;

    float response_time_avg_ms;
    float cpu_usage;
    float memory_usage;
} BackendServer;

typedef struct
{
    uint32_t request_id;
    uint32_t server_id;

    uint32_t source_ip;
    uint16_t source_port;

    uint64_t request_time;
    uint32_t response_time_ms;

    uint32_t request_size;
    uint32_t response_size;
    uint8_t is_sticky;
} RequestEntry;

typedef struct
{
    uint32_t pool_id;
    char pool_name[64];

    BackendServer servers[MAX_BACKEND_SERVERS];
    uint32_t server_count;

    LoadBalancingAlgorithm algorithm;
    uint32_t current_server_index;

    RequestEntry requests[MAX_PENDING_REQUESTS];
    uint32_t request_count;

    uint32_t total_requests;
    uint32_t successful_requests;
    uint32_t failed_requests;

    float pool_throughput_mbps;
} LoadBalancerPool;

typedef struct
{
    uint32_t balancer_id;
    LoadBalancerPool pools[MAX_LOAD_BALANCER_POOLS];
    uint32_t pool_count;

    uint32_t total_servers;
    uint32_t healthy_servers;

    uint64_t total_bytes_processed;
    uint32_t total_connections;
} LoadBalancer;

LoadBalancer *load_balancer_init(void);
int load_balancer_destroy(LoadBalancer *balancer);

uint32_t load_balancer_pool_create(LoadBalancer *balancer, const char *pool_name,
                                   LoadBalancingAlgorithm algorithm);
int load_balancer_pool_destroy(LoadBalancer *balancer, uint32_t pool_id);

int load_balancer_add_server(LoadBalancer *balancer, uint32_t pool_id,
                             uint32_t server_ip, uint16_t server_port, uint32_t weight);
int load_balancer_remove_server(LoadBalancer *balancer, uint32_t pool_id, uint32_t server_id);

int load_balancer_route_request(LoadBalancer *balancer, uint32_t pool_id,
                                uint32_t source_ip, uint16_t source_port);
int load_balancer_select_server(LoadBalancer *balancer, uint32_t pool_id);

int load_balancer_update_server_state(LoadBalancer *balancer, uint32_t pool_id,
                                      uint32_t server_id, ServerState state);
int load_balancer_update_server_metrics(LoadBalancer *balancer, uint32_t pool_id,
                                        uint32_t server_id, float cpu, float memory);

int load_balancer_detect_unhealthy_servers(LoadBalancer *balancer, uint32_t pool_id);
int load_balancer_rebalance_pool(LoadBalancer *balancer, uint32_t pool_id);

int load_balancer_enable_sticky_sessions(LoadBalancer *balancer, uint32_t pool_id);
int load_balancer_disable_sticky_sessions(LoadBalancer *balancer, uint32_t pool_id);

int load_balancer_drain_server(LoadBalancer *balancer, uint32_t pool_id, uint32_t server_id);

typedef struct
{
    uint32_t balancer_id;
    uint32_t total_pools;
    uint32_t total_servers;
    uint32_t healthy_servers;
    uint32_t total_requests;
    uint32_t successful_requests;
    float success_rate;
    float average_response_time_ms;
} LoadBalancerMetrics;

int load_balancer_get_metrics(LoadBalancer *balancer, LoadBalancerMetrics *metrics);

#endif
