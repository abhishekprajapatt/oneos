#include "performance_tuner.h"
#include <stdlib.h>
#include <string.h>

static PerformanceTuner *global_performance_tuner = NULL;
static uint32_t next_profile_id = 1;
static uint32_t next_param_id = 1;
static uint32_t next_rule_id = 1;

PerformanceTuner *performance_tuner_init(void)
{
    PerformanceTuner *tuner = (PerformanceTuner *)malloc(sizeof(PerformanceTuner));
    if (!tuner)
        return NULL;

    memset(tuner, 0, sizeof(PerformanceTuner));
    tuner->tuner_id = 1;
    tuner->profile_count = 0;
    tuner->active_profile_id = 0;
    tuner->tuning_operations = 0;
    tuner->successful_optimizations = 0;
    tuner->overall_performance_improvement = 0.0f;

    global_performance_tuner = tuner;
    return tuner;
}

int performance_tuner_destroy(PerformanceTuner *tuner)
{
    if (!tuner)
        return -1;

    free(tuner);
    global_performance_tuner = NULL;
    return 0;
}

uint32_t tuning_parameter_register(PerformanceTuner *tuner, TuningParameterType type,
                                   uint64_t min_val, uint64_t max_val, uint64_t default_val)
{
    if (!tuner)
        return 0;

    if (tuner->active_profile_id == 0)
        return 0;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        if (tuner->profiles[i].profile_id == tuner->active_profile_id)
        {
            PerformanceProfile_t *profile = &tuner->profiles[i];

            if (profile->parameter_count >= MAX_TUNING_PARAMETERS)
                return 0;

            TuningParameter *param = &profile->parameters[profile->parameter_count];
            memset(param, 0, sizeof(TuningParameter));

            param->parameter_id = next_param_id++;
            param->param_type = type;
            param->current_value = default_val;
            param->min_value = min_val;
            param->max_value = max_val;
            param->default_value = default_val;
            param->is_enabled = 1;
            param->is_adaptive = 0;

            profile->parameter_count++;
            return param->parameter_id;
        }
    }

    return 0;
}

int tuning_parameter_set(PerformanceTuner *tuner, uint32_t param_id, uint64_t value)
{
    if (!tuner || param_id == 0)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        PerformanceProfile_t *profile = &tuner->profiles[i];

        for (uint32_t j = 0; j < profile->parameter_count; j++)
        {
            if (profile->parameters[j].parameter_id == param_id)
            {
                TuningParameter *param = &profile->parameters[j];

                if (value >= param->min_value && value <= param->max_value)
                {
                    param->current_value = value;
                    tuner->tuning_operations++;
                    return 0;
                }

                return -1;
            }
        }
    }

    return -1;
}

int tuning_parameter_get(PerformanceTuner *tuner, uint32_t param_id, uint64_t *value)
{
    if (!tuner || param_id == 0 || !value)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        PerformanceProfile_t *profile = &tuner->profiles[i];

        for (uint32_t j = 0; j < profile->parameter_count; j++)
        {
            if (profile->parameters[j].parameter_id == param_id)
            {
                *value = profile->parameters[j].current_value;
                return 0;
            }
        }
    }

    return -1;
}

uint32_t performance_profile_create(PerformanceTuner *tuner, PerformanceProfile profile_type,
                                    const char *profile_name)
{
    if (!tuner || tuner->profile_count >= MAX_PERFORMANCE_PROFILES)
        return 0;

    PerformanceProfile_t *profile = &tuner->profiles[tuner->profile_count];
    memset(profile, 0, sizeof(PerformanceProfile_t));

    profile->profile_id = next_profile_id++;
    profile->profile_type = profile_type;

    if (profile_name)
    {
        strncpy(profile->profile_name, profile_name, sizeof(profile->profile_name) - 1);
    }

    profile->parameter_count = 0;
    profile->rule_count = 0;
    profile->profile_creation_time = 0;
    profile->is_active = 0;

    tuner->profile_count++;
    return profile->profile_id;
}

int performance_profile_activate(PerformanceTuner *tuner, uint32_t profile_id)
{
    if (!tuner || profile_id == 0)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        if (tuner->profiles[i].profile_id == profile_id)
        {
            tuner->profiles[i].is_active = 1;
            tuner->active_profile_id = profile_id;
            return 0;
        }
    }

    return -1;
}

int performance_profile_deactivate(PerformanceTuner *tuner, uint32_t profile_id)
{
    if (!tuner || profile_id == 0)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        if (tuner->profiles[i].profile_id == profile_id)
        {
            tuner->profiles[i].is_active = 0;
            if (tuner->active_profile_id == profile_id)
            {
                tuner->active_profile_id = 0;
            }
            return 0;
        }
    }

    return -1;
}

int optimization_rule_add(PerformanceTuner *tuner, uint32_t profile_id,
                          TuningParameterType param_type, uint64_t threshold, uint64_t adjusted)
{
    if (!tuner || profile_id == 0)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        if (tuner->profiles[i].profile_id == profile_id)
        {
            PerformanceProfile_t *profile = &tuner->profiles[i];

            if (profile->rule_count >= MAX_OPTIMIZATION_RULES)
                return -1;

            OptimizationRule *rule = &profile->rules[profile->rule_count];
            memset(rule, 0, sizeof(OptimizationRule));

            rule->rule_id = next_rule_id++;
            rule->param_type = param_type;
            rule->threshold_value = threshold;
            rule->adjusted_value = adjusted;
            rule->is_active = 1;

            profile->rule_count++;
            return rule->rule_id;
        }
    }

    return -1;
}

int optimization_rule_remove(PerformanceTuner *tuner, uint32_t profile_id, uint32_t rule_id)
{
    if (!tuner || profile_id == 0 || rule_id == 0)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        if (tuner->profiles[i].profile_id == profile_id)
        {
            PerformanceProfile_t *profile = &tuner->profiles[i];

            for (uint32_t j = 0; j < profile->rule_count; j++)
            {
                if (profile->rules[j].rule_id == rule_id)
                {
                    memmove(&profile->rules[j], &profile->rules[j + 1],
                            sizeof(OptimizationRule) * (profile->rule_count - j - 1));
                    profile->rule_count--;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int optimization_rule_apply(PerformanceTuner *tuner, uint32_t profile_id, uint32_t rule_id)
{
    if (!tuner || profile_id == 0 || rule_id == 0)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        if (tuner->profiles[i].profile_id == profile_id)
        {
            PerformanceProfile_t *profile = &tuner->profiles[i];

            for (uint32_t j = 0; j < profile->rule_count; j++)
            {
                if (profile->rules[j].rule_id == rule_id && profile->rules[j].is_active)
                {
                    tuner->successful_optimizations++;
                    tuner->overall_performance_improvement += 2.5f;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int optimization_detect_hotspots(PerformanceTuner *tuner)
{
    if (!tuner)
        return 0;

    uint32_t hotspot_count = 0;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        PerformanceProfile_t *profile = &tuner->profiles[i];

        if (profile->is_active)
        {
            for (uint32_t j = 0; j < profile->rule_count; j++)
            {
                if (profile->rules[j].is_active)
                {
                    hotspot_count++;
                }
            }
        }
    }

    return hotspot_count;
}

int performance_optimize_for_workload(PerformanceTuner *tuner, uint32_t workload_type)
{
    if (!tuner)
        return -1;

    return 0;
}

int performance_enable_adaptive_tuning(PerformanceTuner *tuner)
{
    if (!tuner)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        PerformanceProfile_t *profile = &tuner->profiles[i];

        for (uint32_t j = 0; j < profile->parameter_count; j++)
        {
            profile->parameters[j].is_adaptive = 1;
        }
    }

    return 0;
}

int performance_disable_adaptive_tuning(PerformanceTuner *tuner)
{
    if (!tuner)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        PerformanceProfile_t *profile = &tuner->profiles[i];

        for (uint32_t j = 0; j < profile->parameter_count; j++)
        {
            profile->parameters[j].is_adaptive = 0;
        }
    }

    return 0;
}

int performance_measure_overhead(PerformanceTuner *tuner, uint64_t *overhead_ns)
{
    if (!tuner || !overhead_ns)
        return -1;

    *overhead_ns = tuner->successful_optimizations * 1000;
    return 0;
}

int performance_tuner_get_metrics(PerformanceTuner *tuner, PerformanceTunerMetrics *metrics)
{
    if (!tuner || !metrics)
        return -1;

    uint32_t active_count = 0;
    uint32_t total_params = 0;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        if (tuner->profiles[i].is_active)
        {
            active_count++;
        }
        total_params += tuner->profiles[i].parameter_count;
    }

    metrics->tuner_id = tuner->tuner_id;
    metrics->active_profiles = active_count;
    metrics->total_parameters = total_params;
    metrics->tuning_operations = tuner->tuning_operations;
    metrics->successful_optimizations = tuner->successful_optimizations;
    metrics->performance_improvement = tuner->overall_performance_improvement;

    return 0;
}
