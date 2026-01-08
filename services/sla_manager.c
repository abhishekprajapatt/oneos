#include "sla_manager.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static SLAManager g_sla_managers[MAX_SLA_MANAGERS] = {0};
static uint32_t g_manager_count = 0;
static uint64_t g_violation_id_counter = 1;
static uint64_t g_action_id_counter = 1;

int sm_create_sla_manager(const char *name)
{
    if (!name || g_manager_count >= MAX_SLA_MANAGERS)
    {
        return -1;
    }

    SLAManager *manager = &g_sla_managers[g_manager_count];
    manager->manager_id = g_manager_count + 1;
    strncpy(manager->name, name, 63);

    manager->sla_count = 0;
    manager->service_level_count = 0;
    manager->violation_count = 0;
    manager->metric_reading_count = 0;
    manager->action_count = 0;

    manager->total_violations = 0;
    manager->total_breaches = 0;
    manager->total_credits_issued = 0.0;
    manager->overall_compliance_score = 100.0;

    manager->created_at = time(NULL);
    manager->is_active = 1;

    g_manager_count++;
    return manager->manager_id;
}

int sm_delete_sla_manager(uint32_t manager_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
    {
        return -1;
    }

    for (uint32_t i = manager_id - 1; i < g_manager_count - 1; i++)
    {
        g_sla_managers[i] = g_sla_managers[i + 1];
    }
    g_manager_count--;

    return 0;
}

uint32_t sm_create_sla_definition(uint32_t manager_id, const char *sla_name,
                                  SLAMetricType metric_type, uint32_t threshold_value,
                                  uint32_t measurement_period_minutes)
{
    if (manager_id == 0 || manager_id > g_manager_count || !sla_name || threshold_value == 0)
    {
        return 0;
    }

    SLAManager *manager = &g_sla_managers[manager_id - 1];
    if (manager->sla_count >= MAX_SLA_DEFINITIONS)
    {
        return 0;
    }

    SLADefinition *sla = &manager->sla_definitions[manager->sla_count];
    sla->sla_id = manager->sla_count + 1;
    strncpy(sla->sla_name, sla_name, 63);
    sla->metric_type = metric_type;
    sla->threshold_value = threshold_value;
    sla->measurement_period_minutes = measurement_period_minutes;
    sla->threshold_type = SLA_THRESHOLD_MINIMUM;
    sla->created_at = time(NULL);
    sla->is_active = 1;
    sla->violation_count = 0;
    sla->last_violation_time = 0;

    manager->sla_count++;
    return sla->sla_id;
}

int sm_delete_sla_definition(uint32_t manager_id, uint32_t sla_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
    {
        return -1;
    }

    SLAManager *manager = &g_sla_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->sla_count; i++)
    {
        if (manager->sla_definitions[i].sla_id == sla_id)
        {
            for (uint32_t j = i; j < manager->sla_count - 1; j++)
            {
                manager->sla_definitions[j] = manager->sla_definitions[j + 1];
            }
            manager->sla_count--;
            return 0;
        }
    }

    return -1;
}

uint32_t sm_create_service_level(uint32_t manager_id, const char *level_name,
                                 uint32_t availability_percentage, uint32_t response_time_ms,
                                 double throughput_tps)
{
    if (manager_id == 0 || manager_id > g_manager_count || !level_name)
    {
        return 0;
    }

    SLAManager *manager = &g_sla_managers[manager_id - 1];
    if (manager->service_level_count >= MAX_SERVICE_LEVELS)
    {
        return 0;
    }

    ServiceLevel *level = &manager->service_levels[manager->service_level_count];
    level->level_id = manager->service_level_count + 1;
    strncpy(level->level_name, level_name, 63);
    level->availability_percentage = availability_percentage;
    level->response_time_ms = response_time_ms;
    level->throughput_tps = throughput_tps;
    level->error_rate_percentage = 0.0;
    level->created_at = time(NULL);
    level->is_active = 1;

    manager->service_level_count++;
    return level->level_id;
}

int sm_delete_service_level(uint32_t manager_id, uint32_t level_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
    {
        return -1;
    }

    SLAManager *manager = &g_sla_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->service_level_count; i++)
    {
        if (manager->service_levels[i].level_id == level_id)
        {
            for (uint32_t j = i; j < manager->service_level_count - 1; j++)
            {
                manager->service_levels[j] = manager->service_levels[j + 1];
            }
            manager->service_level_count--;
            return 0;
        }
    }

    return -1;
}

uint64_t sm_record_metric_reading(uint32_t manager_id, uint32_t sla_id, SLAMetricType metric_type,
                                  uint64_t metric_value, const char *source)
{
    if (manager_id == 0 || manager_id > g_manager_count || !source)
    {
        return 0;
    }

    SLAManager *manager = &g_sla_managers[manager_id - 1];
    if (manager->metric_reading_count >= MAX_METRIC_READINGS)
    {
        return 0;
    }

    SLAMetricReading *reading = &manager->metric_readings[manager->metric_reading_count];
    reading->reading_id = manager->metric_reading_count + 1;
    reading->sla_id = sla_id;
    reading->metric_type = metric_type;
    reading->metric_value = metric_value;
    reading->timestamp = time(NULL);
    strncpy(reading->source, source, 63);

    manager->metric_reading_count++;
    return reading->reading_id;
}

int sm_check_sla_compliance(uint32_t manager_id, uint32_t sla_id, uint64_t current_metric_value,
                            uint8_t *is_compliant)
{
    if (manager_id == 0 || manager_id > g_manager_count || !is_compliant)
    {
        return -1;
    }

    SLAManager *manager = &g_sla_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->sla_count; i++)
    {
        if (manager->sla_definitions[i].sla_id == sla_id)
        {
            SLADefinition *sla = &manager->sla_definitions[i];

            switch (sla->threshold_type)
            {
            case SLA_THRESHOLD_MINIMUM:
                *is_compliant = (current_metric_value >= sla->threshold_value) ? 1 : 0;
                break;
            case SLA_THRESHOLD_MAXIMUM:
                *is_compliant = (current_metric_value <= sla->threshold_value) ? 1 : 0;
                break;
            case SLA_THRESHOLD_RANGE:
                *is_compliant = (current_metric_value >= sla->threshold_value - 10 &&
                                 current_metric_value <= sla->threshold_value + 10)
                                    ? 1
                                    : 0;
                break;
            case SLA_THRESHOLD_EXACT:
                *is_compliant = (current_metric_value == sla->threshold_value) ? 1 : 0;
                break;
            default:
                *is_compliant = 0;
            }

            return 0;
        }
    }

    return -1;
}

uint64_t sm_log_sla_violation(uint32_t manager_id, uint32_t sla_id, uint32_t service_id,
                              uint64_t actual_value, uint64_t expected_value,
                              const char *violation_reason)
{
    if (manager_id == 0 || manager_id > g_manager_count || !violation_reason)
    {
        return 0;
    }

    SLAManager *manager = &g_sla_managers[manager_id - 1];
    if (manager->violation_count >= MAX_VIOLATIONS)
    {
        return 0;
    }

    SLAViolation *violation = &manager->violations[manager->violation_count];
    violation->violation_id = g_violation_id_counter++;
    violation->sla_id = sla_id;
    violation->service_id = service_id;
    violation->actual_value = actual_value;
    violation->expected_value = expected_value;
    violation->violation_time = time(NULL);
    violation->duration_seconds = 0;
    violation->severity = VIOLATION_SEVERITY_WARNING;
    strncpy(violation->violation_reason, violation_reason, 255);
    violation->is_resolved = 0;
    violation->remediation_action_id = 0;

    manager->violation_count++;
    manager->total_violations++;

    for (uint32_t i = 0; i < manager->sla_count; i++)
    {
        if (manager->sla_definitions[i].sla_id == sla_id)
        {
            manager->sla_definitions[i].violation_count++;
            manager->sla_definitions[i].last_violation_time = violation->violation_time;
        }
    }

    return violation->violation_id;
}

int sm_resolve_violation(uint32_t manager_id, uint64_t violation_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
    {
        return -1;
    }

    SLAManager *manager = &g_sla_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->violation_count; i++)
    {
        if (manager->violations[i].violation_id == violation_id)
        {
            manager->violations[i].is_resolved = 1;
            manager->violations[i].duration_seconds = time(NULL) - manager->violations[i].violation_time;
            return 0;
        }
    }

    return -1;
}

int sm_set_violation_severity(uint32_t manager_id, uint64_t violation_id,
                              ViolationSeverity severity)
{
    if (manager_id == 0 || manager_id > g_manager_count)
    {
        return -1;
    }

    SLAManager *manager = &g_sla_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->violation_count; i++)
    {
        if (manager->violations[i].violation_id == violation_id)
        {
            manager->violations[i].severity = severity;
            if (severity == VIOLATION_SEVERITY_BREACH)
                manager->total_breaches++;
            return 0;
        }
    }

    return -1;
}

uint64_t sm_trigger_remediation_action(uint32_t manager_id, uint64_t violation_id,
                                       RemediationActionType action_type, const char *parameters)
{
    if (manager_id == 0 || manager_id > g_manager_count || !parameters)
    {
        return 0;
    }

    SLAManager *manager = &g_sla_managers[manager_id - 1];
    if (manager->action_count >= MAX_REMEDIATION_ACTIONS)
    {
        return 0;
    }

    RemediationAction *action = &manager->remediation_actions[manager->action_count];
    action->action_id = g_action_id_counter++;
    action->violation_id = violation_id;
    action->action_type = action_type;
    action->triggered_at = time(NULL);
    action->completed_at = 0;
    action->success = 0;
    strncpy(action->parameters, parameters, 255);
    strncpy(action->result, "", 255);

    manager->action_count++;

    for (uint32_t i = 0; i < manager->violation_count; i++)
    {
        if (manager->violations[i].violation_id == violation_id)
        {
            manager->violations[i].remediation_action_id = action->action_id;
        }
    }

    return action->action_id;
}

int sm_mark_remediation_complete(uint32_t manager_id, uint64_t action_id, uint8_t success,
                                 const char *result)
{
    if (manager_id == 0 || manager_id > g_manager_count || !result)
    {
        return -1;
    }

    SLAManager *manager = &g_sla_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->action_count; i++)
    {
        if (manager->remediation_actions[i].action_id == action_id)
        {
            manager->remediation_actions[i].completed_at = time(NULL);
            manager->remediation_actions[i].success = success;
            strncpy(manager->remediation_actions[i].result, result, 255);
            return 0;
        }
    }

    return -1;
}

int sm_issue_credit(uint32_t manager_id, uint32_t service_id, double credit_amount,
                    const char *reason)
{
    if (manager_id == 0 || manager_id > g_manager_count || credit_amount <= 0 || !reason)
    {
        return -1;
    }

    SLAManager *manager = &g_sla_managers[manager_id - 1];
    manager->total_credits_issued += credit_amount;

    return 0;
}

int sm_calculate_compliance_score(uint32_t manager_id, uint32_t sla_id, double *score)
{
    if (manager_id == 0 || manager_id > g_manager_count || !score)
    {
        return -1;
    }

    SLAManager *manager = &g_sla_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->sla_count; i++)
    {
        if (manager->sla_definitions[i].sla_id == sla_id)
        {
            SLADefinition *sla = &manager->sla_definitions[i];
            double violation_impact = sla->violation_count * 5.0;
            *score = 100.0 - violation_impact;
            if (*score < 0)
                *score = 0;
            return 0;
        }
    }

    return -1;
}

int sm_get_metric_readings(uint32_t manager_id, SLAMetricReading *readings, uint32_t *count)
{
    if (manager_id == 0 || manager_id > g_manager_count || !readings || !count)
    {
        return -1;
    }

    SLAManager *manager = &g_sla_managers[manager_id - 1];

    if (manager->metric_reading_count > *count)
    {
        return -1;
    }

    memcpy(readings, manager->metric_readings, manager->metric_reading_count * sizeof(SLAMetricReading));
    *count = manager->metric_reading_count;

    return 0;
}

int sm_get_violations(uint32_t manager_id, SLAViolation *violations, uint32_t *count)
{
    if (manager_id == 0 || manager_id > g_manager_count || !violations || !count)
    {
        return -1;
    }

    SLAManager *manager = &g_sla_managers[manager_id - 1];

    if (manager->violation_count > *count)
    {
        return -1;
    }

    memcpy(violations, manager->violations, manager->violation_count * sizeof(SLAViolation));
    *count = manager->violation_count;

    return 0;
}

int sm_get_remediation_actions(uint32_t manager_id, RemediationAction *actions, uint32_t *count)
{
    if (manager_id == 0 || manager_id > g_manager_count || !actions || !count)
    {
        return -1;
    }

    SLAManager *manager = &g_sla_managers[manager_id - 1];

    if (manager->action_count > *count)
    {
        return -1;
    }

    memcpy(actions, manager->remediation_actions, manager->action_count * sizeof(RemediationAction));
    *count = manager->action_count;

    return 0;
}

uint32_t sm_get_total_violations(uint32_t manager_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
        return 0;
    return g_sla_managers[manager_id - 1].total_violations;
}

uint32_t sm_get_total_breaches(uint32_t manager_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
        return 0;
    return g_sla_managers[manager_id - 1].total_breaches;
}

double sm_get_total_credits(uint32_t manager_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
        return 0.0;
    return g_sla_managers[manager_id - 1].total_credits_issued;
}

double sm_get_overall_compliance(uint32_t manager_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
        return 0.0;

    SLAManager *manager = &g_sla_managers[manager_id - 1];

    if (manager->sla_count == 0)
        return 100.0;

    double total_score = 0.0;
    for (uint32_t i = 0; i < manager->sla_count; i++)
    {
        double score;
        sm_calculate_compliance_score(manager_id, manager->sla_definitions[i].sla_id, &score);
        total_score += score;
    }

    return total_score / manager->sla_count;
}

SLAManager *sm_get_manager_info(uint32_t manager_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
        return NULL;
    return &g_sla_managers[manager_id - 1];
}

int sm_generate_compliance_report(uint32_t manager_id, const char *report_filename)
{
    if (manager_id == 0 || manager_id > g_manager_count || !report_filename)
    {
        return -1;
    }

    FILE *f = fopen(report_filename, "w");
    if (!f)
        return -1;

    SLAManager *manager = &g_sla_managers[manager_id - 1];

    fprintf(f, "SLA Compliance Report: %s\n", manager->name);
    fprintf(f, "Total Violations: %u\n", manager->total_violations);
    fprintf(f, "Total Breaches: %u\n", manager->total_breaches);
    fprintf(f, "Total Credits Issued: %.2f\n", manager->total_credits_issued);
    fprintf(f, "Overall Compliance: %.1f%%\n", sm_get_overall_compliance(manager_id));
    fprintf(f, "SLA Definitions: %u\n", manager->sla_count);
    fprintf(f, "Remediation Actions: %u\n", manager->action_count);

    fclose(f);
    return 0;
}
