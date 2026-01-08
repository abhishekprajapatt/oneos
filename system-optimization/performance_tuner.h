#ifndef PERFORMANCE_TUNER_H
#define PERFORMANCE_TUNER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_TUNING_PARAMETERS 512
#define MAX_PERFORMANCE_PROFILES 64
#define MAX_OPTIMIZATION_RULES 256

typedef enum
{
    TUNING_PARAM_CPU_FREQUENCY,
    TUNING_PARAM_MEMORY_THRESHOLD,
    TUNING_PARAM_IO_BUFFER_SIZE,
    TUNING_PARAM_CACHE_SIZE,
    TUNING_PARAM_CONTEXT_SWITCH_OVERHEAD
} TuningParameterType;

typedef enum
{
    PERF_PROFILE_BALANCED,
    PERF_PROFILE_THROUGHPUT,
    PERF_PROFILE_LATENCY,
    PERF_PROFILE_POWER_SAVING,
    PERF_PROFILE_CUSTOM
} PerformanceProfile;

typedef struct
{
    uint32_t parameter_id;
    TuningParameterType param_type;

    uint64_t current_value;
    uint64_t min_value;
    uint64_t max_value;
    uint64_t default_value;

    uint8_t is_enabled;
    uint8_t is_adaptive;
} TuningParameter;

typedef struct
{
    uint32_t rule_id;
    TuningParameterType param_type;
    uint64_t threshold_value;
    uint64_t adjusted_value;

    uint8_t is_active;
} OptimizationRule;

typedef struct
{
    uint32_t profile_id;
    PerformanceProfile profile_type;
    char profile_name[64];

    TuningParameter parameters[MAX_TUNING_PARAMETERS];
    uint32_t parameter_count;

    OptimizationRule rules[MAX_OPTIMIZATION_RULES];
    uint32_t rule_count;

    uint64_t profile_creation_time;
    uint8_t is_active;
} PerformanceProfile_t;

typedef struct
{
    uint32_t tuner_id;
    PerformanceProfile_t profiles[MAX_PERFORMANCE_PROFILES];
    uint32_t profile_count;

    uint32_t active_profile_id;
    uint64_t tuning_operations;
    uint32_t successful_optimizations;
    float overall_performance_improvement;
} PerformanceTuner;

PerformanceTuner *performance_tuner_init(void);
int performance_tuner_destroy(PerformanceTuner *tuner);

uint32_t tuning_parameter_register(PerformanceTuner *tuner, TuningParameterType type,
                                   uint64_t min_val, uint64_t max_val, uint64_t default_val);
int tuning_parameter_set(PerformanceTuner *tuner, uint32_t param_id, uint64_t value);
int tuning_parameter_get(PerformanceTuner *tuner, uint32_t param_id, uint64_t *value);

uint32_t performance_profile_create(PerformanceTuner *tuner, PerformanceProfile profile_type,
                                    const char *profile_name);
int performance_profile_activate(PerformanceTuner *tuner, uint32_t profile_id);
int performance_profile_deactivate(PerformanceTuner *tuner, uint32_t profile_id);

int optimization_rule_add(PerformanceTuner *tuner, uint32_t profile_id,
                          TuningParameterType param_type, uint64_t threshold, uint64_t adjusted);
int optimization_rule_remove(PerformanceTuner *tuner, uint32_t profile_id, uint32_t rule_id);

int optimization_rule_apply(PerformanceTuner *tuner, uint32_t profile_id, uint32_t rule_id);
int optimization_detect_hotspots(PerformanceTuner *tuner);

int performance_optimize_for_workload(PerformanceTuner *tuner, uint32_t workload_type);
int performance_enable_adaptive_tuning(PerformanceTuner *tuner);
int performance_disable_adaptive_tuning(PerformanceTuner *tuner);

int performance_measure_overhead(PerformanceTuner *tuner, uint64_t *overhead_ns);

typedef struct
{
    uint32_t tuner_id;
    uint32_t active_profiles;
    uint32_t total_parameters;
    uint64_t tuning_operations;
    uint32_t successful_optimizations;
    float performance_improvement;
} PerformanceTunerMetrics;

int performance_tuner_get_metrics(PerformanceTuner *tuner, PerformanceTunerMetrics *metrics);

#endif
