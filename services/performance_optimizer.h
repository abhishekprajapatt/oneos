#ifndef PERFORMANCE_OPTIMIZER_H
#define PERFORMANCE_OPTIMIZER_H

#include <stdint.h>
#include <time.h>

#define MAX_OPTIMIZERS 16
#define MAX_OPTIMIZATION_PROFILES 128
#define MAX_PERFORMANCE_METRICS 2048
#define MAX_CACHE_STRATEGIES 256
#define MAX_TUNING_RULES 512
#define MAX_OPTIMIZATION_EVENTS 1024

typedef enum
{
    OPTIMIZATION_MODE_BALANCED = 0,
    OPTIMIZATION_MODE_PERFORMANCE = 1,
    OPTIMIZATION_MODE_POWER_SAVING = 2,
    OPTIMIZATION_MODE_THROUGHPUT = 3,
    OPTIMIZATION_MODE_LATENCY = 4,
    OPTIMIZATION_MODE_CUSTOM = 5
} OptimizationMode;

typedef enum
{
    METRIC_TYPE_CPU_USAGE = 0,
    METRIC_TYPE_MEMORY_USAGE = 1,
    METRIC_TYPE_DISK_IO = 2,
    METRIC_TYPE_NETWORK_IO = 3,
    METRIC_TYPE_CACHE_HIT_RATE = 4,
    METRIC_TYPE_LATENCY = 5,
    METRIC_TYPE_THROUGHPUT = 6
} MetricType;

typedef enum
{
    CACHE_STRATEGY_LRU = 0,
    CACHE_STRATEGY_LFU = 1,
    CACHE_STRATEGY_FIFO = 2,
    CACHE_STRATEGY_ARC = 3,
    CACHE_STRATEGY_CLOCK = 4
} CacheStrategy;

typedef enum
{
    TUNING_ACTION_INCREASE_CACHE = 0,
    TUNING_ACTION_REDUCE_CACHE = 1,
    TUNING_ACTION_ENABLE_COMPRESSION = 2,
    TUNING_ACTION_DISABLE_COMPRESSION = 3,
    TUNING_ACTION_ADJUST_FREQUENCY = 4,
    TUNING_ACTION_PARALLEL_EXECUTION = 5
} TuningAction;

typedef enum
{
    PROFILE_STATUS_ACTIVE = 0,
    PROFILE_STATUS_INACTIVE = 1,
    PROFILE_STATUS_LEARNING = 2,
    PROFILE_STATUS_OPTIMIZED = 3
} ProfileStatus;

typedef struct
{
    uint32_t metric_id;
    MetricType metric_type;
    double current_value;
    double average_value;
    double max_value;
    double min_value;
    uint64_t sample_count;
    time_t last_update;
    uint8_t is_anomalous;
} PerformanceMetric;

typedef struct
{
    uint32_t cache_id;
    char cache_name[63];
    CacheStrategy strategy;
    uint32_t size_mb;
    uint64_t entries;
    uint64_t hits;
    uint64_t misses;
    double hit_rate_percent;
    uint8_t is_enabled;
    time_t created_at;
} CacheStrategy_s;

typedef struct
{
    uint32_t rule_id;
    char rule_name[127];
    char condition[255];
    TuningAction action;
    uint32_t action_param;
    uint32_t trigger_count;
    uint8_t is_enabled;
    uint16_t priority;
    time_t last_triggered;
} TuningRule;

typedef struct
{
    uint32_t event_id;
    char event_name[127];
    TuningAction action_taken;
    uint32_t improvement_percent;
    double performance_delta;
    time_t event_time;
    uint8_t was_successful;
    char reason[255];
} OptimizationEvent;

typedef struct
{
    uint32_t profile_id;
    char profile_name[63];
    OptimizationMode mode;
    ProfileStatus status;
    uint32_t cache_size_mb;
    uint32_t thread_count;
    uint32_t priority_level;
    double target_cpu_percent;
    double target_memory_percent;
    uint32_t optimization_level;
    time_t created_at;
    time_t last_applied;
} OptimizationProfile;

typedef struct
{
    uint32_t optimizer_id;
    char name[63];
    OptimizationMode current_mode;
    uint32_t profile_count;
    uint32_t metric_count;
    uint32_t cache_count;
    uint32_t rule_count;
    uint32_t event_count;
    double cpu_efficiency_percent;
    double memory_efficiency_percent;
    double overall_performance_gain;
    uint64_t total_optimizations;
    uint64_t successful_optimizations;
    time_t last_optimization;
    uint8_t auto_tuning_enabled;
    uint8_t is_active;
    OptimizationProfile profiles[MAX_OPTIMIZATION_PROFILES];
    PerformanceMetric metrics[MAX_PERFORMANCE_METRICS];
    CacheStrategy_s caches[MAX_CACHE_STRATEGIES];
    TuningRule rules[MAX_TUNING_RULES];
    OptimizationEvent events[MAX_OPTIMIZATION_EVENTS];
} PerformanceOptimizer;

int perf_create_optimizer(const char *name);
int perf_delete_optimizer(uint32_t optimizer_id);
int perf_set_optimization_mode(uint32_t optimizer_id, OptimizationMode mode);
int perf_create_optimization_profile(uint32_t optimizer_id, const char *profile_name,
                                   OptimizationMode mode);
int perf_apply_optimization_profile(uint32_t optimizer_id, uint32_t profile_id);
int perf_add_performance_metric(uint32_t optimizer_id, MetricType metric_type);
int perf_update_metric_value(uint32_t optimizer_id, uint32_t metric_id, double value);
int perf_get_metric_statistics(uint32_t optimizer_id, uint32_t metric_id, double *avg, double *max, double *min);
int perf_detect_performance_anomaly(uint32_t optimizer_id, uint32_t metric_id);
int perf_add_cache_strategy(uint32_t optimizer_id, const char *cache_name,
                          CacheStrategy strategy, uint32_t size_mb);
int perf_update_cache_hit_rate(uint32_t optimizer_id, uint32_t cache_id, uint64_t hits, uint64_t misses);
int perf_add_tuning_rule(uint32_t optimizer_id, const char *rule_name,
                       const char *condition, TuningAction action);
int perf_remove_tuning_rule(uint32_t optimizer_id, uint32_t rule_id);
int perf_apply_tuning_action(uint32_t optimizer_id, uint32_t rule_id, TuningAction action);
int perf_analyze_performance(uint32_t optimizer_id);
int perf_optimize_automatically(uint32_t optimizer_id);
int perf_get_optimization_events(uint32_t optimizer_id, OptimizationEvent *events, uint32_t *count);
double perf_get_cpu_efficiency(uint32_t optimizer_id);
double perf_get_memory_efficiency(uint32_t optimizer_id);
double perf_get_overall_performance_gain(uint32_t optimizer_id);
uint64_t perf_get_total_optimizations(uint32_t optimizer_id);
int perf_generate_performance_report(uint32_t optimizer_id, const char *output_path);
int perf_export_optimizer_config(uint32_t optimizer_id, const char *export_path);
int perf_enable_auto_tuning(uint32_t optimizer_id);
int perf_disable_auto_tuning(uint32_t optimizer_id);
PerformanceOptimizer *perf_get_optimizer_info(uint32_t optimizer_id);

#endif // PERFORMANCE_OPTIMIZER_H
