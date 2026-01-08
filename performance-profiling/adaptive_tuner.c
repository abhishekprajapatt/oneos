#include "adaptive_tuner.h"
#include <stdlib.h>
#include <string.h>

static AdaptiveTuner *global_adaptive_tuner = NULL;
static uint32_t next_profile_id = 1;
static uint32_t next_param_id = 1;
static uint32_t next_rule_id = 1;

AdaptiveTuner *adaptive_tuner_init(void)
{
    AdaptiveTuner *tuner = (AdaptiveTuner *)malloc(sizeof(AdaptiveTuner));
    if (!tuner)
        return NULL;

    memset(tuner, 0, sizeof(AdaptiveTuner));
    tuner->tuner_id = 1;
    tuner->profile_count = 0;
    tuner->rule_count = 0;
    tuner->active_profile_id = 0;
    tuner->total_adaptations = 0;
    tuner->successful_tunings = 0;

    global_adaptive_tuner = tuner;
    return tuner;
}

int adaptive_tuner_destroy(AdaptiveTuner *tuner)
{
    if (!tuner)
        return -1;

    free(tuner);
    global_adaptive_tuner = NULL;
    return 0;
}

uint32_t tuning_profile_create(AdaptiveTuner *tuner, const char *profile_name,
                               TuningObjective objective)
{
    if (!tuner || tuner->profile_count >= MAX_TUNING_PROFILES || !profile_name)
        return 0;

    TuningProfile *profile = &tuner->profiles[tuner->profile_count];
    memset(profile, 0, sizeof(TuningProfile));

    profile->profile_id = next_profile_id++;
    strncpy(profile->profile_name, profile_name, sizeof(profile->profile_name) - 1);
    profile->objective = objective;

    profile->param_count = 0;
    profile->creation_time = 0;
    profile->application_count = 0;
    profile->is_active = 0;

    tuner->profile_count++;

    return profile->profile_id;
}

int tuning_profile_destroy(AdaptiveTuner *tuner, uint32_t profile_id)
{
    if (!tuner || profile_id == 0)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        if (tuner->profiles[i].profile_id == profile_id)
        {
            memmove(&tuner->profiles[i], &tuner->profiles[i + 1],
                    sizeof(TuningProfile) * (tuner->profile_count - i - 1));
            tuner->profile_count--;
            return 0;
        }
    }

    return -1;
}

int tuning_profile_activate(AdaptiveTuner *tuner, uint32_t profile_id)
{
    if (!tuner || profile_id == 0)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        tuner->profiles[i].is_active = 0;
    }

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

uint32_t tuning_parameter_create(AdaptiveTuner *tuner, uint32_t profile_id,
                                 const char *param_name, ParameterType type)
{
    if (!tuner || profile_id == 0 || !param_name)
        return 0;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        if (tuner->profiles[i].profile_id == profile_id)
        {
            TuningProfile *profile = &tuner->profiles[i];

            if (profile->param_count >= MAX_TUNING_PARAMETERS)
                return 0;

            TuningParameter *param = &profile->parameters[profile->param_count];
            memset(param, 0, sizeof(TuningParameter));

            param->param_id = next_param_id++;
            strncpy(param->param_name, param_name, sizeof(param->param_name) - 1);
            param->type = type;

            param->current_value = 0;
            param->min_value = 0;
            param->max_value = 100;
            param->default_value = 50;
            param->is_tunable = 1;
            param->requires_restart = 0;

            profile->param_count++;
            return param->param_id;
        }
    }

    return 0;
}

int tuning_parameter_set_value(AdaptiveTuner *tuner, uint32_t profile_id,
                               uint32_t param_id, int64_t value)
{
    if (!tuner || profile_id == 0 || param_id == 0)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        if (tuner->profiles[i].profile_id == profile_id)
        {
            TuningProfile *profile = &tuner->profiles[i];

            for (uint32_t j = 0; j < profile->param_count; j++)
            {
                if (profile->parameters[j].param_id == param_id)
                {
                    if (value >= profile->parameters[j].min_value &&
                        value <= profile->parameters[j].max_value)
                    {
                        profile->parameters[j].current_value = value;
                        return 0;
                    }
                }
            }
        }
    }

    return -1;
}

int tuning_parameter_set_range(AdaptiveTuner *tuner, uint32_t profile_id,
                               uint32_t param_id, int64_t min_val, int64_t max_val)
{
    if (!tuner || profile_id == 0 || param_id == 0)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        if (tuner->profiles[i].profile_id == profile_id)
        {
            TuningProfile *profile = &tuner->profiles[i];

            for (uint32_t j = 0; j < profile->param_count; j++)
            {
                if (profile->parameters[j].param_id == param_id)
                {
                    profile->parameters[j].min_value = min_val;
                    profile->parameters[j].max_value = max_val;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int tuning_parameter_mark_tunable(AdaptiveTuner *tuner, uint32_t profile_id,
                                  uint32_t param_id)
{
    if (!tuner || profile_id == 0 || param_id == 0)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        if (tuner->profiles[i].profile_id == profile_id)
        {
            TuningProfile *profile = &tuner->profiles[i];

            for (uint32_t j = 0; j < profile->param_count; j++)
            {
                if (profile->parameters[j].param_id == param_id)
                {
                    profile->parameters[j].is_tunable = 1;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int tuning_parameter_mark_restart_required(AdaptiveTuner *tuner, uint32_t profile_id,
                                           uint32_t param_id)
{
    if (!tuner || profile_id == 0 || param_id == 0)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        if (tuner->profiles[i].profile_id == profile_id)
        {
            TuningProfile *profile = &tuner->profiles[i];

            for (uint32_t j = 0; j < profile->param_count; j++)
            {
                if (profile->parameters[j].param_id == param_id)
                {
                    profile->parameters[j].requires_restart = 1;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int tuning_parameter_get_value(AdaptiveTuner *tuner, uint32_t profile_id,
                               uint32_t param_id, int64_t *value)
{
    if (!tuner || profile_id == 0 || param_id == 0 || !value)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        if (tuner->profiles[i].profile_id == profile_id)
        {
            TuningProfile *profile = &tuner->profiles[i];

            for (uint32_t j = 0; j < profile->param_count; j++)
            {
                if (profile->parameters[j].param_id == param_id)
                {
                    *value = profile->parameters[j].current_value;
                    return 0;
                }
            }
        }
    }

    return -1;
}

uint32_t adaptive_rule_create(AdaptiveTuner *tuner, const char *rule_name,
                              uint32_t condition_param_id, int64_t condition_threshold,
                              uint32_t target_param_id, int64_t target_value)
{
    if (!tuner || tuner->rule_count >= MAX_ADAPTIVE_RULES || !rule_name)
        return 0;

    AdaptiveRule *rule = &tuner->rules[tuner->rule_count];
    memset(rule, 0, sizeof(AdaptiveRule));

    rule->rule_id = next_rule_id++;
    strncpy(rule->rule_name, rule_name, sizeof(rule->rule_name) - 1);

    rule->condition_param_id = condition_param_id;
    rule->condition_threshold = condition_threshold;
    rule->target_param_id = target_param_id;
    rule->target_value = target_value;
    rule->is_enabled = 0;

    tuner->rule_count++;
    return rule->rule_id;
}

int adaptive_rule_enable(AdaptiveTuner *tuner, uint32_t rule_id)
{
    if (!tuner || rule_id == 0)
        return -1;

    for (uint32_t i = 0; i < tuner->rule_count; i++)
    {
        if (tuner->rules[i].rule_id == rule_id)
        {
            tuner->rules[i].is_enabled = 1;
            return 0;
        }
    }

    return -1;
}

int adaptive_rule_disable(AdaptiveTuner *tuner, uint32_t rule_id)
{
    if (!tuner || rule_id == 0)
        return -1;

    for (uint32_t i = 0; i < tuner->rule_count; i++)
    {
        if (tuner->rules[i].rule_id == rule_id)
        {
            tuner->rules[i].is_enabled = 0;
            return 0;
        }
    }

    return -1;
}

int adaptive_rule_destroy(AdaptiveTuner *tuner, uint32_t rule_id)
{
    if (!tuner || rule_id == 0)
        return -1;

    for (uint32_t i = 0; i < tuner->rule_count; i++)
    {
        if (tuner->rules[i].rule_id == rule_id)
        {
            memmove(&tuner->rules[i], &tuner->rules[i + 1],
                    sizeof(AdaptiveRule) * (tuner->rule_count - i - 1));
            tuner->rule_count--;
            return 0;
        }
    }

    return -1;
}

int apply_adaptive_rules(AdaptiveTuner *tuner)
{
    if (!tuner)
        return -1;

    tuner->total_adaptations++;
    return 0;
}

int apply_tuning_profile(AdaptiveTuner *tuner, uint32_t profile_id)
{
    if (!tuner || profile_id == 0)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        if (tuner->profiles[i].profile_id == profile_id)
        {
            tuner->profiles[i].application_count++;
            tuner->successful_tunings++;
            return 0;
        }
    }

    return -1;
}

int revert_tuning_changes(AdaptiveTuner *tuner)
{
    if (!tuner)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        for (uint32_t j = 0; j < tuner->profiles[i].param_count; j++)
        {
            tuner->profiles[i].parameters[j].current_value =
                tuner->profiles[i].parameters[j].default_value;
        }
    }

    return 0;
}

int tuner_auto_tune(AdaptiveTuner *tuner, TuningObjective objective)
{
    if (!tuner)
        return -1;

    for (uint32_t i = 0; i < tuner->profile_count; i++)
    {
        if (tuner->profiles[i].objective == objective)
        {
            tuning_profile_activate(tuner, tuner->profiles[i].profile_id);
            apply_tuning_profile(tuner, tuner->profiles[i].profile_id);
            return 0;
        }
    }

    return -1;
}

int adaptive_tuner_get_metrics(AdaptiveTuner *tuner, AdaptiveTunerMetrics *metrics)
{
    if (!tuner || !metrics)
        return -1;

    metrics->tuner_id = tuner->tuner_id;
    metrics->active_profile_id = tuner->active_profile_id;
    metrics->total_profiles = tuner->profile_count;

    uint32_t enabled_rules = 0;
    for (uint32_t i = 0; i < tuner->rule_count; i++)
    {
        if (tuner->rules[i].is_enabled)
            enabled_rules++;
    }

    metrics->enabled_rules = enabled_rules;
    metrics->total_adaptations = tuner->total_adaptations;
    metrics->successful_tunings = tuner->successful_tunings;

    return 0;
}
