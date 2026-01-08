#ifndef REAL_TIME_MONITOR_H
#define REAL_TIME_MONITOR_H

#include <stdint.h>
#include <stddef.h>

#define MAX_MONITORING_EVENTS 8192
#define MAX_ALERT_RULES 256
#define MAX_PERFORMANCE_METRICS 512

typedef enum
{
    EVENT_TASK_CREATED,
    EVENT_TASK_STARTED,
    EVENT_TASK_COMPLETED,
    EVENT_CONTEXT_SWITCH,
    EVENT_DEADLINE_MISS,
    EVENT_HIGH_CPU,
    EVENT_HIGH_MEMORY,
    EVENT_ANOMALY
} EventType;

typedef enum
{
    ALERT_NONE,
    ALERT_WARNING,
    ALERT_CRITICAL,
    ALERT_EMERGENCY
} AlertLevel;

typedef struct
{
    uint32_t event_id;
    EventType event_type;
    uint64_t timestamp;
    uint32_t source_id;
    uint64_t event_data;
    char event_description[128];
} MonitoringEvent;

typedef struct
{
    uint32_t rule_id;
    EventType trigger_event;
    AlertLevel alert_level;
    char metric_name[64];
    float threshold_value;
    uint8_t is_active;
} AlertRule;

typedef struct
{
    uint32_t metric_id;
    char metric_name[64];
    float current_value;
    float min_value;
    float max_value;
    float average_value;
    uint64_t sample_count;
    uint64_t last_update_time;
} PerformanceMetric;

typedef struct
{
    uint32_t monitor_id;

    uint32_t event_count;
    MonitoringEvent events[MAX_MONITORING_EVENTS];
    uint32_t event_write_index;

    uint32_t alert_rule_count;
    AlertRule alert_rules[MAX_ALERT_RULES];

    uint32_t metric_count;
    PerformanceMetric metrics[MAX_PERFORMANCE_METRICS];

    uint64_t total_events_logged;
    uint64_t total_alerts_triggered;

    float average_event_latency_us;
    uint8_t is_monitoring_active;
} RealTimeMonitor;

RealTimeMonitor *monitor_init(void);
int monitor_destroy(RealTimeMonitor *monitor);

int monitor_enable(RealTimeMonitor *monitor);
int monitor_disable(RealTimeMonitor *monitor);

int monitor_log_event(RealTimeMonitor *monitor, EventType type, uint32_t source_id, uint64_t data, const char *description);
int monitor_retrieve_events(RealTimeMonitor *monitor, EventType type, MonitoringEvent **events, uint32_t *count);

AlertRule *monitor_add_alert_rule(RealTimeMonitor *monitor, EventType trigger, AlertLevel level, const char *metric_name, float threshold);
int monitor_remove_alert_rule(RealTimeMonitor *monitor, uint32_t rule_id);
int monitor_evaluate_alert_rules(RealTimeMonitor *monitor);

PerformanceMetric *monitor_register_metric(RealTimeMonitor *monitor, const char *metric_name);
int monitor_update_metric(RealTimeMonitor *monitor, uint32_t metric_id, float value);
int monitor_get_metric_stats(RealTimeMonitor *monitor, uint32_t metric_id);

int monitor_detect_anomalies(RealTimeMonitor *monitor);
int monitor_generate_alert(RealTimeMonitor *monitor, AlertLevel level, const char *message);

typedef struct
{
    uint32_t monitor_id;
    uint64_t total_events;
    uint64_t total_alerts;
    float average_latency_us;
    uint32_t active_rules;
    uint32_t registered_metrics;
} MonitorMetrics;

int monitor_get_metrics(RealTimeMonitor *monitor, MonitorMetrics *metrics);

#endif
