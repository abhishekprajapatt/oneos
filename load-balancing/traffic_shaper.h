#ifndef TRAFFIC_SHAPER_H
#define TRAFFIC_SHAPER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_TRAFFIC_POLICIES 128
#define MAX_QUEUES_PER_POLICY 16
#define MAX_TRAFFIC_RULES 512
#define MAX_RATE_LIMITERS 256

typedef enum
{
    TRAFFIC_CLASS_REAL_TIME,
    TRAFFIC_CLASS_INTERACTIVE,
    TRAFFIC_CLASS_BULK,
    TRAFFIC_CLASS_BACKGROUND,
    TRAFFIC_CLASS_BEST_EFFORT
} TrafficClass;

typedef enum
{
    SHAPING_ALGORITHM_TOKEN_BUCKET,
    SHAPING_ALGORITHM_LEAKY_BUCKET,
    SHAPING_ALGORITHM_HIERARCHICAL,
    SHAPING_ALGORITHM_RED
} ShapingAlgorithm;

typedef struct
{
    uint32_t rate_limiter_id;
    TrafficClass traffic_class;

    uint64_t rate_limit_bps;
    uint64_t burst_size_bytes;

    uint64_t tokens;
    uint64_t last_refill_time;

    uint64_t bytes_processed;
    uint32_t packets_dropped;
    uint32_t packets_shaped;
} RateLimiter;

typedef struct
{
    uint32_t rule_id;
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;

    TrafficClass assigned_class;
    uint64_t priority;

    uint8_t is_enabled;
} TrafficRule;

typedef struct
{
    uint32_t queue_id;
    TrafficClass traffic_class;
    uint32_t queue_depth;
    uint32_t max_queue_size;

    uint32_t packets_enqueued;
    uint32_t packets_dequeued;
    uint32_t packets_dropped;
} TrafficQueue;

typedef struct
{
    uint32_t policy_id;
    char policy_name[64];

    ShapingAlgorithm algorithm;

    TrafficQueue queues[MAX_QUEUES_PER_POLICY];
    uint32_t queue_count;

    RateLimiter limiters[MAX_RATE_LIMITERS];
    uint32_t limiter_count;

    TrafficRule rules[MAX_TRAFFIC_RULES];
    uint32_t rule_count;

    uint64_t total_bandwidth_limit_bps;
    uint64_t current_bandwidth_usage_bps;
    float utilization_percent;
} TrafficPolicy;

typedef struct
{
    uint32_t shaper_id;
    TrafficPolicy policies[MAX_TRAFFIC_POLICIES];
    uint32_t policy_count;

    uint64_t total_packets_shaped;
    uint64_t total_packets_dropped;
    uint64_t total_bytes_shaped;
} TrafficShaper;

TrafficShaper *traffic_shaper_init(void);
int traffic_shaper_destroy(TrafficShaper *shaper);

uint32_t traffic_policy_create(TrafficShaper *shaper, const char *policy_name,
                               ShapingAlgorithm algorithm, uint64_t total_bandwidth_bps);
int traffic_policy_destroy(TrafficShaper *shaper, uint32_t policy_id);

int traffic_rule_add(TrafficShaper *shaper, uint32_t policy_id,
                     uint32_t src_ip, uint32_t dst_ip, uint16_t src_port, uint16_t dst_port,
                     TrafficClass traffic_class);
int traffic_rule_remove(TrafficShaper *shaper, uint32_t policy_id, uint32_t rule_id);

int traffic_rate_limiter_create(TrafficShaper *shaper, uint32_t policy_id,
                                TrafficClass traffic_class, uint64_t rate_bps, uint64_t burst_bytes);

int traffic_classify_packet(TrafficShaper *shaper, uint32_t policy_id,
                            uint32_t src_ip, uint32_t dst_ip);

int traffic_shape_packet(TrafficShaper *shaper, uint32_t policy_id,
                         uint32_t packet_size, TrafficClass traffic_class);

int traffic_enqueue_packet(TrafficShaper *shaper, uint32_t policy_id,
                           uint32_t queue_id, uint32_t packet_size);
int traffic_dequeue_packet(TrafficShaper *shaper, uint32_t policy_id, uint32_t queue_id);

int traffic_enforce_rate_limits(TrafficShaper *shaper, uint32_t policy_id);
int traffic_update_bandwidth_usage(TrafficShaper *shaper, uint32_t policy_id);

typedef struct
{
    uint32_t shaper_id;
    uint32_t active_policies;
    uint64_t total_packets_shaped;
    uint64_t total_packets_dropped;
    uint64_t total_bytes_shaped;
    float average_utilization_percent;
} TrafficShaperMetrics;

int traffic_shaper_get_metrics(TrafficShaper *shaper, TrafficShaperMetrics *metrics);

#endif
