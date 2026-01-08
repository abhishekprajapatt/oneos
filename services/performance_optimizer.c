#include "performance_optimizer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static PerformanceOptimizer g_optimizers[MAX_OPTIMIZERS] = {0};
static uint32_t g_optimizer_count = 0;

int perf_create_optimizer(const char *name)
{
    if (!name || g_optimizer_count >= MAX_OPTIMIZERS)
    {
        return -1;
    }

    PerformanceOptimizer *optimizer = &g_optimizers[g_optimizer_count];
    optimizer->optimizer_id = g_optimizer_count + 1;
    strncpy(optimizer->name, name, 62);

    optimizer->current_mode = OPTIMIZATION_MODE_BALANCED;
    optimizer->profile_count = 0;
    optimizer->metric_count = 0;
    optimizer->cache_count = 0;
    optimizer->rule_count = 0;
    optimizer->event_count = 0;

    optimizer->cpu_efficiency_percent = 50.0;
    optimizer->memory_efficiency_percent = 50.0;
    optimizer->overall_performance_gain = 0.0;
    optimizer->total_optimizations = 0;
    optimizer->successful_optimizations = 0;
    optimizer->last_optimization = 0;
    optimizer->auto_tuning_enabled = 0;
    optimizer->is_active = 1;

    g_optimizer_count++;
    return optimizer->optimizer_id;
}

int perf_delete_optimizer(uint32_t optimizer_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return -1;
    }

    for (uint32_t i = optimizer_id - 1; i < g_optimizer_count - 1; i++)
    {
        g_optimizers[i] = g_optimizers[i + 1];
    }
    g_optimizer_count--;

    return 0;
}

int perf_set_optimization_mode(uint32_t optimizer_id, OptimizationMode mode)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return -1;
    }

    PerformanceOptimizer *optimizer = &g_optimizers[optimizer_id - 1];
    optimizer->current_mode = mode;

    return 0;
}

int perf_create_optimization_profile(uint32_t optimizer_id, const char *profile_name,
                                     OptimizationMode mode)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || !profile_name)
    {
        return -1;
    }

    PerformanceOptimizer *optimizer = &g_optimizers[optimizer_id - 1];
    if (optimizer->profile_count >= MAX_OPTIMIZATION_PROFILES)
    {
        return -1;
    }

    OptimizationProfile *profile = &optimizer->profiles[optimizer->profile_count];
    profile->profile_id = optimizer->profile_count + 1;
    strncpy(profile->profile_name, profile_name, 62);
    profile->mode = mode;
    profile->status = PROFILE_STATUS_LEARNING;
    profile->cache_size_mb = 256;
    profile->thread_count = 4;
    profile->priority_level = 5;
    profile->target_cpu_percent = 75.0;
    profile->target_memory_percent = 70.0;
    profile->optimization_level = 5;
    profile->created_at = time(NULL);
    profile->last_applied = 0;

    optimizer->profile_count++;

    return profile->profile_id;
}

int perf_apply_optimization_profile(uint32_t optimizer_id, uint32_t profile_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return -1;
    }

    PerformanceOptimizer *optimizer = &g_optimizers[optimizer_id - 1];

    for (uint32_t i = 0; i < optimizer->profile_count; i++)
    {
        if (optimizer->profiles[i].profile_id == profile_id)
        {
            optimizer->profiles[i].status = PROFILE_STATUS_ACTIVE;
            optimizer->profiles[i].last_applied = time(NULL);
            optimizer->current_mode = optimizer->profiles[i].mode;
            return 0;
        }
    }

    return -1;
}

int perf_add_performance_metric(uint32_t optimizer_id, MetricType metric_type)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return -1;
    }

    PerformanceOptimizer *optimizer = &g_optimizers[optimizer_id - 1];
    if (optimizer->metric_count >= MAX_PERFORMANCE_METRICS)
    {
        return -1;
    }

    PerformanceMetric *metric = &optimizer->metrics[optimizer->metric_count];
    metric->metric_id = optimizer->metric_count + 1;
    metric->metric_type = metric_type;
    metric->current_value = 0.0;
    metric->average_value = 0.0;
    metric->max_value = 0.0;
    metric->min_value = 100.0;
    metric->sample_count = 0;
    metric->last_update = time(NULL);
    metric->is_anomalous = 0;

    optimizer->metric_count++;

    return metric->metric_id;
}

int perf_update_metric_value(uint32_t optimizer_id, uint32_t metric_id, double value)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return -1;
    }

    PerformanceOptimizer *optimizer = &g_optimizers[optimizer_id - 1];

    for (uint32_t i = 0; i < optimizer->metric_count; i++)
    {
        if (optimizer->metrics[i].metric_id == metric_id)
        {
            optimizer->metrics[i].current_value = value;
            optimizer->metrics[i].sample_count++;
            optimizer->metrics[i].last_update = time(NULL);

            if (optimizer->metrics[i].sample_count == 1)
            {
                optimizer->metrics[i].average_value = value;
                optimizer->metrics[i].max_value = value;
                optimizer->metrics[i].min_value = value;
            }
            else
            {
                optimizer->metrics[i].average_value =
                    (optimizer->metrics[i].average_value * (optimizer->metrics[i].sample_count - 1) + value) /
                    optimizer->metrics[i].sample_count;

                if (value > optimizer->metrics[i].max_value)
                    optimizer->metrics[i].max_value = value;
                if (value < optimizer->metrics[i].min_value)
                    optimizer->metrics[i].min_value = value;
            }

            return 0;
        }
    }

    return -1;
}

int perf_get_metric_statistics(uint32_t optimizer_id, uint32_t metric_id, double *avg, double *max, double *min)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || !avg || !max || !min)
    {
        return -1;
    }

    PerformanceOptimizer *optimizer = &g_optimizers[optimizer_id - 1];

    for (uint32_t i = 0; i < optimizer->metric_count; i++)
    {
        if (optimizer->metrics[i].metric_id == metric_id)
        {
            *avg = optimizer->metrics[i].average_value;
            *max = optimizer->metrics[i].max_value;
            *min = optimizer->metrics[i].min_value;
            return 0;
        }
    }

    return -1;
}

int perf_detect_performance_anomaly(uint32_t optimizer_id, uint32_t metric_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return -1;
    }

    PerformanceOptimizer *optimizer = &g_optimizers[optimizer_id - 1];

    for (uint32_t i = 0; i < optimizer->metric_count; i++)
    {
        if (optimizer->metrics[i].metric_id == metric_id)
        {
            double stddev = 0.0;
            if (optimizer->metrics[i].sample_count > 1)
            {
                double variance = (optimizer->metrics[i].max_value - optimizer->metrics[i].min_value) / 2;
                stddev = sqrt(variance);
            }

            double threshold = optimizer->metrics[i].average_value + (2 * stddev);

            if (optimizer->metrics[i].current_value > threshold)
            {
                optimizer->metrics[i].is_anomalous = 1;
                return 1;
            }

            optimizer->metrics[i].is_anomalous = 0;
            return 0;
        }
    }

    return -1;
}

int perf_add_cache_strategy(uint32_t optimizer_id, const char *cache_name,
                            CacheStrategy strategy, uint32_t size_mb)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || !cache_name)
    {
        return -1;
    }

    PerformanceOptimizer *optimizer = &g_optimizers[optimizer_id - 1];
    if (optimizer->cache_count >= MAX_CACHE_STRATEGIES)
    {
        return -1;
    }

    CacheStrategy_s *cache = &optimizer->caches[optimizer->cache_count];
    cache->cache_id = optimizer->cache_count + 1;
    strncpy(cache->cache_name, cache_name, 62);
    cache->strategy = strategy;
    cache->size_mb = size_mb;
    cache->entries = (uint64_t)size_mb * 1024 / 8;
    cache->hits = 0;
    cache->misses = 0;
    cache->hit_rate_percent = 0.0;
    cache->is_enabled = 1;
    cache->created_at = time(NULL);

    optimizer->cache_count++;

    return cache->cache_id;
}

int perf_update_cache_hit_rate(uint32_t optimizer_id, uint32_t cache_id, uint64_t hits, uint64_t misses)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return -1;
    }

    PerformanceOptimizer *optimizer = &g_optimizers[optimizer_id - 1];

    for (uint32_t i = 0; i < optimizer->cache_count; i++)
    {
        if (optimizer->caches[i].cache_id == cache_id)
        {
            optimizer->caches[i].hits = hits;
            optimizer->caches[i].misses = misses;

            uint64_t total = hits + misses;
            if (total > 0)
            {
                optimizer->caches[i].hit_rate_percent = (double)hits / total * 100.0;
            }

            return 0;
        }
    }

    return -1;
}

int perf_add_tuning_rule(uint32_t optimizer_id, const char *rule_name,
                         const char *condition, TuningAction action)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || !rule_name || !condition)
    {
        return -1;
    }

    PerformanceOptimizer *optimizer = &g_optimizers[optimizer_id - 1];
    if (optimizer->rule_count >= MAX_TUNING_RULES)
    {
        return -1;
    }

    TuningRule *rule = &optimizer->rules[optimizer->rule_count];
    rule->rule_id = optimizer->rule_count + 1;
    strncpy(rule->rule_name, rule_name, 126);
    strncpy(rule->condition, condition, 254);
    rule->action = action;
    rule->action_param = 0;
    rule->trigger_count = 0;
    rule->is_enabled = 1;
    rule->priority = 100;
    rule->last_triggered = 0;

    optimizer->rule_count++;

    return rule->rule_id;
}

int perf_remove_tuning_rule(uint32_t optimizer_id, uint32_t rule_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return -1;
    }

    PerformanceOptimizer *optimizer = &g_optimizers[optimizer_id - 1];

    for (uint32_t i = 0; i < optimizer->rule_count; i++)
    {
        if (optimizer->rules[i].rule_id == rule_id)
        {
            for (uint32_t j = i; j < optimizer->rule_count - 1; j++)
            {
                optimizer->rules[j] = optimizer->rules[j + 1];
            }
            optimizer->rule_count--;
            return 0;
        }
    }

    return -1;
}

int perf_apply_tuning_action(uint32_t optimizer_id, uint32_t rule_id, TuningAction action)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return -1;
    }

    PerformanceOptimizer *optimizer = &g_optimizers[optimizer_id - 1];

    for (uint32_t i = 0; i < optimizer->rule_count; i++)
    {
        if (optimizer->rules[i].rule_id == rule_id)
        {
            optimizer->rules[i].trigger_count++;
            optimizer->rules[i].last_triggered = time(NULL);

            OptimizationEvent *event = &optimizer->events[optimizer->event_count];
            if (optimizer->event_count < MAX_OPTIMIZATION_EVENTS)
            {
                event->event_id = optimizer->event_count + 1;
                snprintf(event->event_name, 126, "TUNING_EVENT_%u", event->event_id);
                event->action_taken = action;
                event->improvement_percent = 5 + (action % 15);
                event->performance_delta = 1.05;
                event->event_time = time(NULL);
                event->was_successful = 1;
                strcpy(event->reason, "Auto-tuning applied");

                optimizer->event_count++;
                optimizer->total_optimizations++;
                optimizer->successful_optimizations++;
            }

            return 0;
        }
    }

    return -1;
}

int perf_analyze_performance(uint32_t optimizer_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return -1;
    }

    PerformanceOptimizer *optimizer = &g_optimizers[optimizer_id - 1];

    double total_cpu_efficiency = 0.0;
    double total_memory_efficiency = 0.0;

    for (uint32_t i = 0; i < optimizer->metric_count; i++)
    {
        if (optimizer->metrics[i].metric_type == METRIC_TYPE_CPU_USAGE)
        {
            total_cpu_efficiency = 100.0 - optimizer->metrics[i].current_value;
        }
        else if (optimizer->metrics[i].metric_type == METRIC_TYPE_MEMORY_USAGE)
        {
            total_memory_efficiency = 100.0 - optimizer->metrics[i].current_value;
        }
    }

    optimizer->cpu_efficiency_percent = total_cpu_efficiency;
    optimizer->memory_efficiency_percent = total_memory_efficiency;
    optimizer->overall_performance_gain = (total_cpu_efficiency + total_memory_efficiency) / 2.0;

    return 0;
}

int perf_optimize_automatically(uint32_t optimizer_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return -1;
    }

    PerformanceOptimizer *optimizer = &g_optimizers[optimizer_id - 1];

    for (uint32_t i = 0; i < optimizer->rule_count; i++)
    {
        if (optimizer->rules[i].is_enabled)
        {
            perf_apply_tuning_action(optimizer_id, optimizer->rules[i].rule_id, optimizer->rules[i].action);
        }
    }

    optimizer->last_optimization = time(NULL);

    return 0;
}

int perf_get_optimization_events(uint32_t optimizer_id, OptimizationEvent *events, uint32_t *count)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || !events || !count)
    {
        return -1;
    }

    PerformanceOptimizer *optimizer = &g_optimizers[optimizer_id - 1];

    if (optimizer->event_count > *count)
    {
        return -1;
    }

    memcpy(events, optimizer->events, optimizer->event_count * sizeof(OptimizationEvent));
    *count = optimizer->event_count;

    return 0;
}

double perf_get_cpu_efficiency(uint32_t optimizer_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return 0.0;
    }

    return g_optimizers[optimizer_id - 1].cpu_efficiency_percent;
}

double perf_get_memory_efficiency(uint32_t optimizer_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return 0.0;
    }

    return g_optimizers[optimizer_id - 1].memory_efficiency_percent;
}

double perf_get_overall_performance_gain(uint32_t optimizer_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return 0.0;
    }

    return g_optimizers[optimizer_id - 1].overall_performance_gain;
}

uint64_t perf_get_total_optimizations(uint32_t optimizer_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return 0;
    }

    return g_optimizers[optimizer_id - 1].total_optimizations;
}

int perf_generate_performance_report(uint32_t optimizer_id, const char *output_path)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || !output_path)
    {
        return -1;
    }

    return 0;
}

int perf_export_optimizer_config(uint32_t optimizer_id, const char *export_path)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || !export_path)
    {
        return -1;
    }

    return 0;
}

int perf_enable_auto_tuning(uint32_t optimizer_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return -1;
    }

    g_optimizers[optimizer_id - 1].auto_tuning_enabled = 1;
    return 0;
}

int perf_disable_auto_tuning(uint32_t optimizer_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return -1;
    }

    g_optimizers[optimizer_id - 1].auto_tuning_enabled = 0;
    return 0;
}

PerformanceOptimizer *perf_get_optimizer_info(uint32_t optimizer_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
    {
        return NULL;
    }

    return &g_optimizers[optimizer_id - 1];
}
