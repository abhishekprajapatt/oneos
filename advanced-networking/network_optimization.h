#ifndef NETWORK_OPTIMIZATION_H
#define NETWORK_OPTIMIZATION_H

#include <stdint.h>
#include <stddef.h>

#define MAX_TRAFFIC_POLICIES 64
#define MAX_BANDWIDTH_LIMITERS 128
#define MAX_QOS_CLASSES 8
#define MAX_ROUTE_ENTRIES 1024

typedef enum
{
    QOS_CLASS_BEST_EFFORT,
    QOS_CLASS_STANDARD,
    QOS_CLASS_PREMIUM,
    QOS_CLASS_PRIORITY,
    QOS_CLASS_VOICE,
    QOS_CLASS_VIDEO,
    QOS_CLASS_CRITICAL
} QoSClass;

typedef enum
{
    TRAFFIC_POLICY_FIFO,
    TRAFFIC_POLICY_PRIORITY_QUEUE,
    TRAFFIC_POLICY_WEIGHTED_FAIR,
    TRAFFIC_POLICY_RATE_LIMIT,
    TRAFFIC_POLICY_TRAFFIC_SHAPING
} TrafficPolicyType;

typedef struct
{
    uint32_t policy_id;
    TrafficPolicyType policy_type;

    uint64_t max_bandwidth; // bits per second
    uint64_t burst_size;

    uint32_t priority;
    QoSClass qos_class;

    uint64_t packets_processed;
    uint64_t packets_dropped;
    uint64_t bytes_processed;

    float congestion_level;
} TrafficPolicy;

typedef struct
{
    uint32_t limiter_id;
    char interface_name[32];

    uint64_t bandwidth_limit; // bps
    uint64_t current_bandwidth;
    uint64_t peak_bandwidth;

    uint32_t burst_packets;
    uint32_t current_burst;

    uint32_t enabled;
    uint32_t violation_count;
} BandwidthLimiter;

typedef struct
{
    uint32_t route_id;
    char destination_ip[64];
    uint32_t prefix_length;

    char next_hop_ip[64];
    uint32_t interface_id;

    uint32_t metric;
    uint32_t cost;

    uint64_t packets_routed;
    uint32_t last_used_time;
} RouteEntry;

typedef struct
{
    uint32_t optimizer_id;

    uint32_t traffic_policy_count;
    TrafficPolicy traffic_policies[MAX_TRAFFIC_POLICIES];

    uint32_t bandwidth_limiter_count;
    BandwidthLimiter bandwidth_limiters[MAX_BANDWIDTH_LIMITERS];

    uint32_t qos_class_count;
    QoSClass qos_classes[MAX_QOS_CLASSES];

    uint32_t route_entry_count;
    RouteEntry route_entries[MAX_ROUTE_ENTRIES];

    // Statistics
    uint64_t total_packets_optimized;
    uint64_t total_packets_dropped;
    uint64_t total_latency_reduction;

    float average_throughput_mbps;
    float average_latency_ms;
} NetworkOptimizer;

// Optimizer initialization
NetworkOptimizer *network_optimizer_init(void);
int network_optimizer_destroy(NetworkOptimizer *opt);

// Traffic policy management
int optimizer_add_traffic_policy(NetworkOptimizer *opt, TrafficPolicyType type,
                                 uint64_t bandwidth, uint32_t priority);
int optimizer_remove_traffic_policy(NetworkOptimizer *opt, uint32_t policy_id);
int optimizer_apply_traffic_policy(NetworkOptimizer *opt, uint32_t policy_id);

// Bandwidth limiting
int optimizer_add_bandwidth_limiter(NetworkOptimizer *opt, const char *interface, uint64_t bandwidth);
int optimizer_remove_bandwidth_limiter(NetworkOptimizer *opt, uint32_t limiter_id);
int optimizer_check_bandwidth_limit(NetworkOptimizer *opt, uint32_t limiter_id, uint32_t packet_size);

// QoS management
int optimizer_add_qos_class(NetworkOptimizer *opt, QoSClass class, uint32_t priority);
int optimizer_classify_packet(NetworkOptimizer *opt, const uint8_t *packet, uint32_t size, QoSClass *class);
int optimizer_schedule_packet(NetworkOptimizer *opt, uint8_t *packet, uint32_t size, QoSClass class);

// Routing
int optimizer_add_route(NetworkOptimizer *opt, const char *destination, uint32_t prefix_len,
                        const char *next_hop, uint32_t metric);
int optimizer_remove_route(NetworkOptimizer *opt, uint32_t route_id);
int optimizer_lookup_route(NetworkOptimizer *opt, const char *destination_ip, RouteEntry **route);
int optimizer_update_route_metrics(NetworkOptimizer *opt, uint32_t route_id, uint32_t new_metric);

// Congestion control
int optimizer_detect_congestion(NetworkOptimizer *opt);
int optimizer_handle_congestion(NetworkOptimizer *opt);
int optimizer_reduce_congestion(NetworkOptimizer *opt);

// Load balancing
int optimizer_balance_load(NetworkOptimizer *opt);
int optimizer_distribute_traffic(NetworkOptimizer *opt);

// Monitoring
typedef struct
{
    uint32_t optimizer_id;
    uint32_t active_policies;
    uint32_t active_limiters;
    float average_throughput_mbps;
    float average_latency_ms;
    float congestion_level;
    uint64_t total_packets_optimized;
} NetworkOptimizerMetrics;

int optimizer_get_metrics(NetworkOptimizer *opt, NetworkOptimizerMetrics *metrics);

#endif // NETWORK_OPTIMIZATION_H
