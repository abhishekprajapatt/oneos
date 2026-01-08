#ifndef HEALTH_MONITOR_H
#define HEALTH_MONITOR_H

#include <stdint.h>
#include <stddef.h>

#define MAX_HEALTH_CHECKS 512
#define MAX_HEALTH_METRICS 256
#define MAX_ALERT_RULES 128

typedef enum
{
    HEALTH_STATE_HEALTHY,
    HEALTH_STATE_DEGRADED,
    HEALTH_STATE_CRITICAL,
    HEALTH_STATE_FAILED
} HealthState;

typedef enum
{
    HEALTH_CHECK_CPU_LOAD,
    HEALTH_CHECK_MEMORY_USAGE,
    HEALTH_CHECK_DISK_USAGE,
    HEALTH_CHECK_PROCESS_ALIVE,
    HEALTH_CHECK_SERVICE_RESPONSIVE,
    HEALTH_CHECK_NETWORK_CONNECTIVITY
} HealthCheckType;

typedef struct
{
    uint32_t check_id;
    HealthCheckType check_type;
    HealthState state;

    uint64_t last_check_time;
    uint32_t check_interval_seconds;

    float threshold_warn;
    float threshold_critical;
    float current_value;

    uint32_t consecutive_failures;
    uint32_t total_checks;
    uint32_t failed_checks;
} HealthCheck;

typedef struct
{
    uint32_t metric_id;
    HealthCheckType metric_type;

    float min_value;
    float max_value;
    float average_value;
    float current_value;

    uint64_t total_samples;
    uint32_t sample_count;
} HealthMetric;

typedef struct
{
    uint32_t rule_id;
    HealthCheckType check_type;
    float condition_threshold;
    HealthState trigger_state;

    uint8_t is_enabled;
    uint32_t alert_priority;
} AlertRule;

typedef struct
{
    uint32_t monitor_id;
    HealthCheck checks[MAX_HEALTH_CHECKS];
    uint32_t check_count;

    HealthMetric metrics[MAX_HEALTH_METRICS];
    uint32_t metric_count;

    AlertRule rules[MAX_ALERT_RULES];
    uint32_t rule_count;

    HealthState overall_health;
    uint32_t total_alerts;
    uint32_t critical_alerts;
} HealthMonitor;

HealthMonitor *health_monitor_init(void);
int health_monitor_destroy(HealthMonitor *monitor);

uint32_t health_check_register(HealthMonitor *monitor, HealthCheckType type,
                               uint32_t interval_seconds, float warn_threshold, float crit_threshold);
int health_check_unregister(HealthMonitor *monitor, uint32_t check_id);

int health_check_execute(HealthMonitor *monitor, uint32_t check_id, float current_value);
int health_update_metric(HealthMonitor *monitor, uint32_t metric_id, float value);

int health_alert_rule_add(HealthMonitor *monitor, HealthCheckType type,
                          float threshold, HealthState state, uint32_t priority);
int health_alert_rule_remove(HealthMonitor *monitor, uint32_t rule_id);

int health_evaluate_all_checks(HealthMonitor *monitor);
int health_update_overall_state(HealthMonitor *monitor);

int health_generate_report(HealthMonitor *monitor);

typedef struct
{
    uint32_t monitor_id;
    HealthState overall_health;
    uint32_t healthy_checks;
    uint32_t degraded_checks;
    uint32_t critical_checks;
    uint32_t total_alerts;
} HealthMetrics;

int health_get_metrics(HealthMonitor *monitor, HealthMetrics *metrics);

#endif
