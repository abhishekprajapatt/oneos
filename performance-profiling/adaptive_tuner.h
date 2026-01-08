#ifndef ADAPTIVE_TUNER_H
#define ADAPTIVE_TUNER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_TUNING_PARAMETERS 256
#define MAX_TUNING_PROFILES 32
#define MAX_ADAPTIVE_RULES 128
#define MAX_PARAMETER_VALUES 16

typedef enum
{
    PARAM_TYPE_INTEGER,
    PARAM_TYPE_BOOLEAN,
    PARAM_TYPE_ENUM,
    PARAM_TYPE_FLOAT
} ParameterType;

typedef enum
{
    TUNING_OBJECTIVE_PERFORMANCE,
    TUNING_OBJECTIVE_POWER,
    TUNING_OBJECTIVE_LATENCY,
    TUNING_OBJECTIVE_THROUGHPUT,
    TUNING_OBJECTIVE_BALANCED
} TuningObjective;

typedef struct
{
    uint32_t param_id;
    char param_name[128];
    ParameterType type;

    int64_t current_value;
    int64_t min_value;
    int64_t max_value;
    int64_t default_value;

    uint8_t is_tunable;
    uint8_t requires_restart;
} TuningParameter;

typedef struct
{
    uint32_t profile_id;
    char profile_name[128];
    TuningObjective objective;

    TuningParameter parameters[MAX_TUNING_PARAMETERS];
    uint32_t param_count;

    uint64_t creation_time;
    uint32_t application_count;
    uint8_t is_active;
} TuningProfile;

typedef struct
{
    uint32_t rule_id;
    char rule_name[128];

    uint32_t condition_param_id;
    int64_t condition_threshold;

    uint32_t target_param_id;
    int64_t target_value;

    uint8_t is_enabled;
} AdaptiveRule;

typedef struct
{
    uint32_t tuner_id;

    TuningProfile profiles[MAX_TUNING_PROFILES];
    uint32_t profile_count;

    AdaptiveRule rules[MAX_ADAPTIVE_RULES];
    uint32_t rule_count;

    uint32_t active_profile_id;
    uint64_t total_adaptations;
    uint64_t successful_tunings;
} AdaptiveTuner;

AdaptiveTuner *adaptive_tuner_init(void);
int adaptive_tuner_destroy(AdaptiveTuner *tuner);

uint32_t tuning_profile_create(AdaptiveTuner *tuner, const char *profile_name,
                               TuningObjective objective);

int tuning_profile_destroy(AdaptiveTuner *tuner, uint32_t profile_id);

int tuning_profile_activate(AdaptiveTuner *tuner, uint32_t profile_id);

uint32_t tuning_parameter_create(AdaptiveTuner *tuner, uint32_t profile_id,
                                 const char *param_name, ParameterType type);

int tuning_parameter_set_value(AdaptiveTuner *tuner, uint32_t profile_id,
                               uint32_t param_id, int64_t value);

int tuning_parameter_set_range(AdaptiveTuner *tuner, uint32_t profile_id,
                               uint32_t param_id, int64_t min_val, int64_t max_val);

int tuning_parameter_mark_tunable(AdaptiveTuner *tuner, uint32_t profile_id,
                                  uint32_t param_id);

int tuning_parameter_mark_restart_required(AdaptiveTuner *tuner, uint32_t profile_id,
                                           uint32_t param_id);

int tuning_parameter_get_value(AdaptiveTuner *tuner, uint32_t profile_id,
                               uint32_t param_id, int64_t *value);

uint32_t adaptive_rule_create(AdaptiveTuner *tuner, const char *rule_name,
                              uint32_t condition_param_id, int64_t condition_threshold,
                              uint32_t target_param_id, int64_t target_value);

int adaptive_rule_enable(AdaptiveTuner *tuner, uint32_t rule_id);

int adaptive_rule_disable(AdaptiveTuner *tuner, uint32_t rule_id);

int adaptive_rule_destroy(AdaptiveTuner *tuner, uint32_t rule_id);

int apply_adaptive_rules(AdaptiveTuner *tuner);

int apply_tuning_profile(AdaptiveTuner *tuner, uint32_t profile_id);

int revert_tuning_changes(AdaptiveTuner *tuner);

int tuner_auto_tune(AdaptiveTuner *tuner, TuningObjective objective);

typedef struct
{
    uint32_t tuner_id;
    uint32_t active_profile_id;
    uint32_t total_profiles;
    uint32_t enabled_rules;
    uint64_t total_adaptations;
    uint64_t successful_tunings;
} AdaptiveTunerMetrics;

int adaptive_tuner_get_metrics(AdaptiveTuner *tuner, AdaptiveTunerMetrics *metrics);

#endif
