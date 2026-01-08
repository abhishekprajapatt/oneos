#include "traffic_shaper.h"
#include <stdlib.h>
#include <string.h>

static TrafficShaper *global_traffic_shaper = NULL;
static uint32_t next_policy_id = 1;
static uint32_t next_rule_id = 1;
static uint32_t next_limiter_id = 1;

TrafficShaper *traffic_shaper_init(void)
{
    TrafficShaper *shaper = (TrafficShaper *)malloc(sizeof(TrafficShaper));
    if (!shaper)
        return NULL;

    memset(shaper, 0, sizeof(TrafficShaper));
    shaper->shaper_id = 1;
    shaper->policy_count = 0;
    shaper->total_packets_shaped = 0;
    shaper->total_packets_dropped = 0;
    shaper->total_bytes_shaped = 0;

    global_traffic_shaper = shaper;
    return shaper;
}

int traffic_shaper_destroy(TrafficShaper *shaper)
{
    if (!shaper)
        return -1;

    free(shaper);
    global_traffic_shaper = NULL;
    return 0;
}

uint32_t traffic_policy_create(TrafficShaper *shaper, const char *policy_name,
                               ShapingAlgorithm algorithm, uint64_t total_bandwidth_bps)
{
    if (!shaper || shaper->policy_count >= MAX_TRAFFIC_POLICIES)
        return 0;

    TrafficPolicy *policy = &shaper->policies[shaper->policy_count];
    memset(policy, 0, sizeof(TrafficPolicy));

    policy->policy_id = next_policy_id++;

    if (policy_name)
    {
        strncpy(policy->policy_name, policy_name, sizeof(policy->policy_name) - 1);
    }

    policy->algorithm = algorithm;
    policy->queue_count = 0;
    policy->limiter_count = 0;
    policy->rule_count = 0;
    policy->total_bandwidth_limit_bps = total_bandwidth_bps;
    policy->current_bandwidth_usage_bps = 0;
    policy->utilization_percent = 0.0f;

    shaper->policy_count++;
    return policy->policy_id;
}

int traffic_policy_destroy(TrafficShaper *shaper, uint32_t policy_id)
{
    if (!shaper || policy_id == 0)
        return -1;

    for (uint32_t i = 0; i < shaper->policy_count; i++)
    {
        if (shaper->policies[i].policy_id == policy_id)
        {
            memmove(&shaper->policies[i], &shaper->policies[i + 1],
                    sizeof(TrafficPolicy) * (shaper->policy_count - i - 1));
            shaper->policy_count--;
            return 0;
        }
    }

    return -1;
}

int traffic_rule_add(TrafficShaper *shaper, uint32_t policy_id,
                     uint32_t src_ip, uint32_t dst_ip, uint16_t src_port, uint16_t dst_port,
                     TrafficClass traffic_class)
{
    if (!shaper || policy_id == 0)
        return -1;

    for (uint32_t i = 0; i < shaper->policy_count; i++)
    {
        if (shaper->policies[i].policy_id == policy_id)
        {
            TrafficPolicy *policy = &shaper->policies[i];

            if (policy->rule_count >= MAX_TRAFFIC_RULES)
                return -1;

            TrafficRule *rule = &policy->rules[policy->rule_count];
            memset(rule, 0, sizeof(TrafficRule));

            rule->rule_id = next_rule_id++;
            rule->src_ip = src_ip;
            rule->dst_ip = dst_ip;
            rule->src_port = src_port;
            rule->dst_port = dst_port;
            rule->assigned_class = traffic_class;
            rule->priority = policy->rule_count;
            rule->is_enabled = 1;

            policy->rule_count++;
            return rule->rule_id;
        }
    }

    return -1;
}

int traffic_rule_remove(TrafficShaper *shaper, uint32_t policy_id, uint32_t rule_id)
{
    if (!shaper || policy_id == 0 || rule_id == 0)
        return -1;

    for (uint32_t i = 0; i < shaper->policy_count; i++)
    {
        if (shaper->policies[i].policy_id == policy_id)
        {
            TrafficPolicy *policy = &shaper->policies[i];

            for (uint32_t j = 0; j < policy->rule_count; j++)
            {
                if (policy->rules[j].rule_id == rule_id)
                {
                    memmove(&policy->rules[j], &policy->rules[j + 1],
                            sizeof(TrafficRule) * (policy->rule_count - j - 1));
                    policy->rule_count--;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int traffic_rate_limiter_create(TrafficShaper *shaper, uint32_t policy_id,
                                TrafficClass traffic_class, uint64_t rate_bps, uint64_t burst_bytes)
{
    if (!shaper || policy_id == 0)
        return -1;

    for (uint32_t i = 0; i < shaper->policy_count; i++)
    {
        if (shaper->policies[i].policy_id == policy_id)
        {
            TrafficPolicy *policy = &shaper->policies[i];

            if (policy->limiter_count >= MAX_RATE_LIMITERS)
                return -1;

            RateLimiter *limiter = &policy->limiters[policy->limiter_count];
            memset(limiter, 0, sizeof(RateLimiter));

            limiter->rate_limiter_id = next_limiter_id++;
            limiter->traffic_class = traffic_class;
            limiter->rate_limit_bps = rate_bps;
            limiter->burst_size_bytes = burst_bytes;
            limiter->tokens = burst_bytes;
            limiter->last_refill_time = 0;
            limiter->bytes_processed = 0;
            limiter->packets_dropped = 0;
            limiter->packets_shaped = 0;

            policy->limiter_count++;
            return limiter->rate_limiter_id;
        }
    }

    return -1;
}

int traffic_classify_packet(TrafficShaper *shaper, uint32_t policy_id,
                            uint32_t src_ip, uint32_t dst_ip)
{
    if (!shaper || policy_id == 0)
        return -1;

    for (uint32_t i = 0; i < shaper->policy_count; i++)
    {
        if (shaper->policies[i].policy_id == policy_id)
        {
            TrafficPolicy *policy = &shaper->policies[i];

            for (uint32_t j = 0; j < policy->rule_count; j++)
            {
                if (policy->rules[j].src_ip == src_ip && policy->rules[j].dst_ip == dst_ip)
                {
                    return policy->rules[j].assigned_class;
                }
            }

            return TRAFFIC_CLASS_BEST_EFFORT;
        }
    }

    return -1;
}

int traffic_shape_packet(TrafficShaper *shaper, uint32_t policy_id,
                         uint32_t packet_size, TrafficClass traffic_class)
{
    if (!shaper || policy_id == 0)
        return -1;

    for (uint32_t i = 0; i < shaper->policy_count; i++)
    {
        if (shaper->policies[i].policy_id == policy_id)
        {
            TrafficPolicy *policy = &shaper->policies[i];

            for (uint32_t j = 0; j < policy->limiter_count; j++)
            {
                if (policy->limiters[j].traffic_class == traffic_class)
                {
                    RateLimiter *limiter = &policy->limiters[j];

                    if (limiter->tokens >= (uint64_t)packet_size)
                    {
                        limiter->tokens -= packet_size;
                        limiter->bytes_processed += packet_size;
                        limiter->packets_shaped++;
                        shaper->total_bytes_shaped += packet_size;
                        shaper->total_packets_shaped++;
                        return 0;
                    }
                    else
                    {
                        limiter->packets_dropped++;
                        shaper->total_packets_dropped++;
                        return -1;
                    }
                }
            }

            return 0;
        }
    }

    return -1;
}

int traffic_enqueue_packet(TrafficShaper *shaper, uint32_t policy_id,
                           uint32_t queue_id, uint32_t packet_size)
{
    if (!shaper || policy_id == 0 || queue_id == 0)
        return -1;

    for (uint32_t i = 0; i < shaper->policy_count; i++)
    {
        if (shaper->policies[i].policy_id == policy_id)
        {
            TrafficPolicy *policy = &shaper->policies[i];

            for (uint32_t j = 0; j < policy->queue_count; j++)
            {
                if (policy->queues[j].queue_id == queue_id)
                {
                    TrafficQueue *queue = &policy->queues[j];

                    if (queue->queue_depth < queue->max_queue_size)
                    {
                        queue->queue_depth++;
                        queue->packets_enqueued++;
                        return 0;
                    }
                    else
                    {
                        queue->packets_dropped++;
                        return -1;
                    }
                }
            }
        }
    }

    return -1;
}

int traffic_dequeue_packet(TrafficShaper *shaper, uint32_t policy_id, uint32_t queue_id)
{
    if (!shaper || policy_id == 0 || queue_id == 0)
        return -1;

    for (uint32_t i = 0; i < shaper->policy_count; i++)
    {
        if (shaper->policies[i].policy_id == policy_id)
        {
            TrafficPolicy *policy = &shaper->policies[i];

            for (uint32_t j = 0; j < policy->queue_count; j++)
            {
                if (policy->queues[j].queue_id == queue_id)
                {
                    TrafficQueue *queue = &policy->queues[j];

                    if (queue->queue_depth > 0)
                    {
                        queue->queue_depth--;
                        queue->packets_dequeued++;
                        return 0;
                    }
                }
            }
        }
    }

    return -1;
}

int traffic_enforce_rate_limits(TrafficShaper *shaper, uint32_t policy_id)
{
    if (!shaper || policy_id == 0)
        return -1;

    return 0;
}

int traffic_update_bandwidth_usage(TrafficShaper *shaper, uint32_t policy_id)
{
    if (!shaper || policy_id == 0)
        return -1;

    for (uint32_t i = 0; i < shaper->policy_count; i++)
    {
        if (shaper->policies[i].policy_id == policy_id)
        {
            TrafficPolicy *policy = &shaper->policies[i];

            uint64_t total_bytes = 0;
            for (uint32_t j = 0; j < policy->limiter_count; j++)
            {
                total_bytes += policy->limiters[j].bytes_processed;
            }

            policy->current_bandwidth_usage_bps = total_bytes * 8;

            if (policy->total_bandwidth_limit_bps > 0)
            {
                policy->utilization_percent = (float)policy->current_bandwidth_usage_bps /
                                              (float)policy->total_bandwidth_limit_bps * 100.0f;
            }

            return 0;
        }
    }

    return -1;
}

int traffic_shaper_get_metrics(TrafficShaper *shaper, TrafficShaperMetrics *metrics)
{
    if (!shaper || !metrics)
        return -1;

    float total_util = 0.0f;

    for (uint32_t i = 0; i < shaper->policy_count; i++)
    {
        total_util += shaper->policies[i].utilization_percent;
    }

    metrics->shaper_id = shaper->shaper_id;
    metrics->active_policies = shaper->policy_count;
    metrics->total_packets_shaped = shaper->total_packets_shaped;
    metrics->total_packets_dropped = shaper->total_packets_dropped;
    metrics->total_bytes_shaped = shaper->total_bytes_shaped;

    if (shaper->policy_count > 0)
    {
        metrics->average_utilization_percent = total_util / shaper->policy_count;
    }
    else
    {
        metrics->average_utilization_percent = 0.0f;
    }

    return 0;
}
