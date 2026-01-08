#include "health_monitor.h"
#include <stdlib.h>
#include <string.h>

static HealthMonitor *global_health_monitor = NULL;
static uint32_t next_check_id = 1;
static uint32_t next_metric_id = 1;
static uint32_t next_rule_id = 1;

HealthMonitor *health_monitor_init(void)
{
    HealthMonitor *monitor = (HealthMonitor *)malloc(sizeof(HealthMonitor));
    if (!monitor)
        return NULL;

    memset(monitor, 0, sizeof(HealthMonitor));
    monitor->monitor_id = 1;
    monitor->check_count = 0;
    monitor->metric_count = 0;
    monitor->rule_count = 0;
    monitor->overall_health = HEALTH_STATE_HEALTHY;
    monitor->total_alerts = 0;
    monitor->critical_alerts = 0;

    global_health_monitor = monitor;
    return monitor;
}

int health_monitor_destroy(HealthMonitor *monitor)
{
    if (!monitor)
        return -1;

    free(monitor);
    global_health_monitor = NULL;
    return 0;
}

uint32_t health_check_register(HealthMonitor *monitor, HealthCheckType type,
                               uint32_t interval_seconds, float warn_threshold, float crit_threshold)
{
    if (!monitor || monitor->check_count >= MAX_HEALTH_CHECKS)
        return 0;

    HealthCheck *check = &monitor->checks[monitor->check_count];
    memset(check, 0, sizeof(HealthCheck));

    check->check_id = next_check_id++;
    check->check_type = type;
    check->state = HEALTH_STATE_HEALTHY;
    check->last_check_time = 0;
    check->check_interval_seconds = (interval_seconds > 0) ? interval_seconds : 60;
    check->threshold_warn = warn_threshold;
    check->threshold_critical = crit_threshold;
    check->current_value = 0.0f;
    check->consecutive_failures = 0;
    check->total_checks = 0;
    check->failed_checks = 0;

    monitor->check_count++;
    return check->check_id;
}

int health_check_unregister(HealthMonitor *monitor, uint32_t check_id)
{
    if (!monitor || check_id == 0)
        return -1;

    for (uint32_t i = 0; i < monitor->check_count; i++)
    {
        if (monitor->checks[i].check_id == check_id)
        {
            memmove(&monitor->checks[i], &monitor->checks[i + 1],
                    sizeof(HealthCheck) * (monitor->check_count - i - 1));
            monitor->check_count--;
            return 0;
        }
    }

    return -1;
}

int health_check_execute(HealthMonitor *monitor, uint32_t check_id, float current_value)
{
    if (!monitor || check_id == 0)
        return -1;

    for (uint32_t i = 0; i < monitor->check_count; i++)
    {
        if (monitor->checks[i].check_id == check_id)
        {
            HealthCheck *check = &monitor->checks[i];
            check->current_value = current_value;
            check->last_check_time = 0;
            check->total_checks++;

            if (current_value >= check->threshold_critical)
            {
                check->state = HEALTH_STATE_CRITICAL;
                check->failed_checks++;
                check->consecutive_failures++;
                monitor->total_alerts++;
                monitor->critical_alerts++;
            }
            else if (current_value >= check->threshold_warn)
            {
                check->state = HEALTH_STATE_DEGRADED;
                check->consecutive_failures++;
                monitor->total_alerts++;
            }
            else
            {
                check->state = HEALTH_STATE_HEALTHY;
                check->consecutive_failures = 0;
            }

            return 0;
        }
    }

    return -1;
}

int health_update_metric(HealthMonitor *monitor, uint32_t metric_id, float value)
{
    if (!monitor || metric_id == 0)
        return -1;

    for (uint32_t i = 0; i < monitor->metric_count; i++)
    {
        if (monitor->metrics[i].metric_id == metric_id)
        {
            HealthMetric *metric = &monitor->metrics[i];
            metric->current_value = value;
            metric->total_samples += value;
            metric->sample_count++;

            if (value < metric->min_value || metric->min_value == 0)
            {
                metric->min_value = value;
            }
            if (value > metric->max_value)
            {
                metric->max_value = value;
            }

            if (metric->sample_count > 0)
            {
                metric->average_value = (float)metric->total_samples / metric->sample_count;
            }

            return 0;
        }
    }

    return -1;
}

int health_alert_rule_add(HealthMonitor *monitor, HealthCheckType type,
                          float threshold, HealthState state, uint32_t priority)
{
    if (!monitor || monitor->rule_count >= MAX_ALERT_RULES)
        return -1;

    AlertRule *rule = &monitor->rules[monitor->rule_count];
    memset(rule, 0, sizeof(AlertRule));

    rule->rule_id = next_rule_id++;
    rule->check_type = type;
    rule->condition_threshold = threshold;
    rule->trigger_state = state;
    rule->is_enabled = 1;
    rule->alert_priority = priority;

    monitor->rule_count++;
    return rule->rule_id;
}

int health_alert_rule_remove(HealthMonitor *monitor, uint32_t rule_id)
{
    if (!monitor || rule_id == 0)
        return -1;

    for (uint32_t i = 0; i < monitor->rule_count; i++)
    {
        if (monitor->rules[i].rule_id == rule_id)
        {
            memmove(&monitor->rules[i], &monitor->rules[i + 1],
                    sizeof(AlertRule) * (monitor->rule_count - i - 1));
            monitor->rule_count--;
            return 0;
        }
    }

    return -1;
}

int health_evaluate_all_checks(HealthMonitor *monitor)
{
    if (!monitor)
        return -1;

    uint32_t critical_count = 0;

    for (uint32_t i = 0; i < monitor->check_count; i++)
    {
        HealthCheck *check = &monitor->checks[i];

        for (uint32_t j = 0; j < monitor->rule_count; j++)
        {
            AlertRule *rule = &monitor->rules[j];
            if (rule->is_enabled && rule->check_type == check->check_type)
            {
                if (check->current_value >= rule->condition_threshold)
                {
                    if (rule->trigger_state == HEALTH_STATE_CRITICAL)
                    {
                        critical_count++;
                    }
                }
            }
        }
    }

    return critical_count;
}

int health_update_overall_state(HealthMonitor *monitor)
{
    if (!monitor)
        return -1;

    uint32_t healthy = 0, degraded = 0, critical = 0;

    for (uint32_t i = 0; i < monitor->check_count; i++)
    {
        if (monitor->checks[i].state == HEALTH_STATE_HEALTHY)
            healthy++;
        else if (monitor->checks[i].state == HEALTH_STATE_DEGRADED)
            degraded++;
        else if (monitor->checks[i].state == HEALTH_STATE_CRITICAL)
            critical++;
    }

    if (critical > 0)
    {
        monitor->overall_health = HEALTH_STATE_CRITICAL;
    }
    else if (degraded > 0)
    {
        monitor->overall_health = HEALTH_STATE_DEGRADED;
    }
    else
    {
        monitor->overall_health = HEALTH_STATE_HEALTHY;
    }

    return 0;
}

int health_generate_report(HealthMonitor *monitor)
{
    if (!monitor)
        return -1;

    return monitor->check_count;
}

int health_get_metrics(HealthMonitor *monitor, HealthMetrics *metrics)
{
    if (!monitor || !metrics)
        return -1;

    uint32_t healthy = 0, degraded = 0, critical = 0;

    for (uint32_t i = 0; i < monitor->check_count; i++)
    {
        if (monitor->checks[i].state == HEALTH_STATE_HEALTHY)
            healthy++;
        else if (monitor->checks[i].state == HEALTH_STATE_DEGRADED)
            degraded++;
        else if (monitor->checks[i].state == HEALTH_STATE_CRITICAL)
            critical++;
    }

    metrics->monitor_id = monitor->monitor_id;
    metrics->overall_health = monitor->overall_health;
    metrics->healthy_checks = healthy;
    metrics->degraded_checks = degraded;
    metrics->critical_checks = critical;
    metrics->total_alerts = monitor->total_alerts;

    return 0;
}
