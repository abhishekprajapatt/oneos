#include "real_time_monitor.h"
#include <stdlib.h>
#include <string.h>

static RealTimeMonitor *global_monitor = NULL;
static uint32_t next_event_id = 1;
static uint32_t next_rule_id = 1;
static uint32_t next_metric_id = 1;

RealTimeMonitor *monitor_init(void)
{
    RealTimeMonitor *monitor = (RealTimeMonitor *)malloc(sizeof(RealTimeMonitor));
    if (!monitor)
        return NULL;

    memset(monitor, 0, sizeof(RealTimeMonitor));
    monitor->monitor_id = 1;
    monitor->event_count = 0;
    monitor->event_write_index = 0;
    monitor->alert_rule_count = 0;
    monitor->metric_count = 0;
    monitor->total_events_logged = 0;
    monitor->total_alerts_triggered = 0;
    monitor->average_event_latency_us = 0.0f;
    monitor->is_monitoring_active = 0;

    global_monitor = monitor;
    return monitor;
}

int monitor_destroy(RealTimeMonitor *monitor)
{
    if (!monitor)
        return -1;

    free(monitor);
    global_monitor = NULL;
    return 0;
}

int monitor_enable(RealTimeMonitor *monitor)
{
    if (!monitor)
        return -1;

    monitor->is_monitoring_active = 1;
    return 0;
}

int monitor_disable(RealTimeMonitor *monitor)
{
    if (!monitor)
        return -1;

    monitor->is_monitoring_active = 0;
    return 0;
}

int monitor_log_event(RealTimeMonitor *monitor, EventType type, uint32_t source_id, uint64_t data, const char *description)
{
    if (!monitor || !description || monitor->event_count >= MAX_MONITORING_EVENTS)
        return -1;

    if (!monitor->is_monitoring_active)
        return -1;

    uint32_t idx = monitor->event_write_index;
    MonitoringEvent *event = &monitor->events[idx];

    event->event_id = next_event_id++;
    event->event_type = type;
    event->timestamp = 0;
    event->source_id = source_id;
    event->event_data = data;
    strncpy(event->event_description, description, 127);

    monitor->event_write_index = (monitor->event_write_index + 1) % MAX_MONITORING_EVENTS;
    monitor->event_count = (monitor->event_count + 1 > MAX_MONITORING_EVENTS) ? MAX_MONITORING_EVENTS : (monitor->event_count + 1);
    monitor->total_events_logged++;

    return 0;
}

int monitor_retrieve_events(RealTimeMonitor *monitor, EventType type, MonitoringEvent **events, uint32_t *count)
{
    if (!monitor || !events || !count)
        return -1;

    *count = 0;
    for (uint32_t i = 0; i < monitor->event_count; i++)
    {
        if (monitor->events[i].event_type == type)
        {
            events[(*count)++] = &monitor->events[i];
            if (*count >= 100)
                break;
        }
    }

    return 0;
}

AlertRule *monitor_add_alert_rule(RealTimeMonitor *monitor, EventType trigger, AlertLevel level, const char *metric_name, float threshold)
{
    if (!monitor || monitor->alert_rule_count >= MAX_ALERT_RULES || !metric_name)
        return NULL;

    AlertRule *rule = &monitor->alert_rules[monitor->alert_rule_count];
    memset(rule, 0, sizeof(AlertRule));

    rule->rule_id = next_rule_id++;
    rule->trigger_event = trigger;
    rule->alert_level = level;
    strncpy(rule->metric_name, metric_name, 63);
    rule->threshold_value = threshold;
    rule->is_active = 1;

    monitor->alert_rule_count++;
    return rule;
}

int monitor_remove_alert_rule(RealTimeMonitor *monitor, uint32_t rule_id)
{
    if (!monitor)
        return -1;

    for (uint32_t i = 0; i < monitor->alert_rule_count; i++)
    {
        if (monitor->alert_rules[i].rule_id == rule_id)
        {
            monitor->alert_rules[i].is_active = 0;
            return 0;
        }
    }

    return -1;
}

int monitor_evaluate_alert_rules(RealTimeMonitor *monitor)
{
    if (!monitor)
        return -1;

    for (uint32_t i = 0; i < monitor->alert_rule_count; i++)
    {
        if (monitor->alert_rules[i].is_active)
        {
            for (uint32_t j = 0; j < monitor->metric_count; j++)
            {
                if (strcmp(monitor->alert_rules[i].metric_name, monitor->metrics[j].metric_name) == 0)
                {
                    if (monitor->metrics[j].current_value > monitor->alert_rules[i].threshold_value)
                    {
                        monitor->total_alerts_triggered++;
                    }
                }
            }
        }
    }

    return 0;
}

PerformanceMetric *monitor_register_metric(RealTimeMonitor *monitor, const char *metric_name)
{
    if (!monitor || monitor->metric_count >= MAX_PERFORMANCE_METRICS || !metric_name)
        return NULL;

    PerformanceMetric *metric = &monitor->metrics[monitor->metric_count];
    memset(metric, 0, sizeof(PerformanceMetric));

    metric->metric_id = next_metric_id++;
    strncpy(metric->metric_name, metric_name, 63);
    metric->current_value = 0.0f;
    metric->min_value = 1e10f;
    metric->max_value = 0.0f;
    metric->average_value = 0.0f;
    metric->sample_count = 0;
    metric->last_update_time = 0;

    monitor->metric_count++;
    return metric;
}

int monitor_update_metric(RealTimeMonitor *monitor, uint32_t metric_id, float value)
{
    if (!monitor)
        return -1;

    for (uint32_t i = 0; i < monitor->metric_count; i++)
    {
        if (monitor->metrics[i].metric_id == metric_id)
        {
            monitor->metrics[i].current_value = value;
            monitor->metrics[i].min_value = (value < monitor->metrics[i].min_value) ? value : monitor->metrics[i].min_value;
            monitor->metrics[i].max_value = (value > monitor->metrics[i].max_value) ? value : monitor->metrics[i].max_value;

            if (monitor->metrics[i].sample_count > 0)
            {
                monitor->metrics[i].average_value = (monitor->metrics[i].average_value * monitor->metrics[i].sample_count + value) /
                                                    (monitor->metrics[i].sample_count + 1);
            }
            else
            {
                monitor->metrics[i].average_value = value;
            }

            monitor->metrics[i].sample_count++;
            monitor->metrics[i].last_update_time = 0;

            return 0;
        }
    }

    return -1;
}

int monitor_get_metric_stats(RealTimeMonitor *monitor, uint32_t metric_id)
{
    if (!monitor)
        return -1;

    for (uint32_t i = 0; i < monitor->metric_count; i++)
    {
        if (monitor->metrics[i].metric_id == metric_id)
        {
            return 0;
        }
    }

    return -1;
}

int monitor_detect_anomalies(RealTimeMonitor *monitor)
{
    if (!monitor)
        return -1;

    for (uint32_t i = 0; i < monitor->metric_count; i++)
    {
        PerformanceMetric *metric = &monitor->metrics[i];

        float deviation = (metric->current_value - metric->average_value);
        if (deviation < 0.0f)
            deviation = -deviation;

        if (deviation > (metric->average_value * 2.0f))
        {
            monitor_log_event(monitor, EVENT_ANOMALY, i, (uint64_t)metric->current_value, "Anomaly detected");
        }
    }

    return 0;
}

int monitor_generate_alert(RealTimeMonitor *monitor, AlertLevel level, const char *message)
{
    if (!monitor || !message)
        return -1;

    return 0;
}

int monitor_get_metrics(RealTimeMonitor *monitor, MonitorMetrics *metrics)
{
    if (!monitor || !metrics)
        return -1;

    metrics->monitor_id = monitor->monitor_id;
    metrics->total_events = monitor->total_events_logged;
    metrics->total_alerts = monitor->total_alerts_triggered;
    metrics->average_latency_us = monitor->average_event_latency_us;
    metrics->active_rules = 0;
    metrics->registered_metrics = monitor->metric_count;

    for (uint32_t i = 0; i < monitor->alert_rule_count; i++)
    {
        if (monitor->alert_rules[i].is_active)
        {
            metrics->active_rules++;
        }
    }

    return 0;
}
