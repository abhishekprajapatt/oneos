#ifndef ONEOS_SLA_MANAGER_H
#define ONEOS_SLA_MANAGER_H

#include <stdint.h>
#include <time.h>

#define MAX_SLA_MANAGERS 16
#define MAX_SLA_DEFINITIONS 128
#define MAX_SERVICE_LEVELS 64
#define MAX_SLA_VIOLATIONS 2048
#define MAX_SLA_METRICS 4096
#define MAX_REMEDIATION_ACTIONS 512

typedef enum
{
    SLA_METRIC_AVAILABILITY = 0,
    SLA_METRIC_RESPONSE_TIME = 1,
    SLA_METRIC_THROUGHPUT = 2,
    SLA_METRIC_ERROR_RATE = 3,
    SLA_METRIC_CPU_UTILIZATION = 4,
    SLA_METRIC_MEMORY_UTILIZATION = 5,
    SLA_METRIC_DISK_UTILIZATION = 6,
    SLA_METRIC_NETWORK_LATENCY = 7
} SLAMetricType;

typedef enum
{
    SLA_THRESHOLD_MINIMUM = 0,
    SLA_THRESHOLD_MAXIMUM = 1,
    SLA_THRESHOLD_RANGE = 2,
    SLA_THRESHOLD_EXACT = 3
} SLAThresholdType;

typedef enum
{
    VIOLATION_SEVERITY_WARNING = 0,
    VIOLATION_SEVERITY_CRITICAL = 1,
    VIOLATION_SEVERITY_BREACH = 2
} ViolationSeverity;

typedef enum
{
    REMEDIATION_ACTION_SCALE_UP = 0,
    REMEDIATION_ACTION_SCALE_DOWN = 1,
    REMEDIATION_ACTION_FAILOVER = 2,
    REMEDIATION_ACTION_LOAD_BALANCE = 3,
    REMEDIATION_ACTION_CACHE_REFRESH = 4,
    REMEDIATION_ACTION_PRIORITY_BOOST = 5,
    REMEDIATION_ACTION_ALERT = 6
} RemediationActionType;

typedef struct
{
    uint32_t metric_id;
    SLAMetricType metric_type;
    char metric_name[64];
    SLAThresholdType threshold_type;
    double min_threshold;
    double max_threshold;
    double warning_threshold;
    uint32_t measurement_interval_seconds;
    uint8_t is_critical;
} SLAMetricDefinition;

typedef struct
{
    uint32_t level_id;
    char level_name[64];
    double availability_percentage;
    uint32_t max_response_time_ms;
    uint32_t min_throughput_ops;
    double max_error_rate;
    char description[256];
    time_t effective_from;
} ServiceLevel;

typedef struct
{
    uint32_t sla_id;
    char sla_name[64];
    char service_name[64];
    uint32_t owner_id;
    ServiceLevel service_level;
    uint32_t metric_count;
    SLAMetricDefinition metrics[MAX_SLA_METRICS];

    time_t sla_period_start;
    time_t sla_period_end;

    double measured_availability;
    double measured_response_time;
    uint32_t measured_throughput;
    double measured_error_rate;

    uint8_t is_compliant;
    uint32_t violations_count;
    double compliance_percentage;

    double penalty_amount;
    double credit_amount;

    time_t created_at;
    time_t last_checked;
} SLADefinition;

typedef struct
{
    uint64_t violation_id;
    uint32_t sla_id;
    SLAMetricType metric_type;
    ViolationSeverity severity;
    double expected_value;
    double actual_value;
    time_t violation_start;
    time_t violation_end;
    uint32_t violation_duration_seconds;
    uint8_t is_acknowledged;
    uint8_t is_remediated;
    char description[256];
} SLAViolation;

typedef struct
{
    uint32_t action_id;
    uint64_t violation_id;
    RemediationActionType action_type;
    char action_description[256];
    time_t action_triggered;
    time_t action_completed;
    uint8_t was_successful;
    char result_details[512];
} RemediationAction;

typedef struct
{
    uint64_t metric_reading_id;
    uint32_t sla_id;
    SLAMetricType metric_type;
    double metric_value;
    time_t measurement_time;
    uint8_t is_within_threshold;
    uint8_t is_warning;
} SLAMetricReading;

typedef struct
{
    uint32_t manager_id;
    char name[64];

    uint32_t sla_definition_count;
    SLADefinition sla_definitions[MAX_SLA_DEFINITIONS];

    uint32_t service_level_count;
    ServiceLevel service_levels[MAX_SERVICE_LEVELS];

    uint32_t violation_count;
    SLAViolation violations[MAX_SLA_VIOLATIONS];

    uint32_t metric_reading_count;
    SLAMetricReading metric_readings[MAX_SLA_METRICS];

    uint32_t remediation_action_count;
    RemediationAction remediation_actions[MAX_REMEDIATION_ACTIONS];

    uint64_t total_sla_breaches;
    uint64_t total_violations_detected;
    uint64_t total_violations_remediated;

    double total_penalties;
    double total_credits_issued;
    double average_remediation_time_minutes;

    double overall_compliance_percentage;

    uint8_t auto_remediation_enabled;
    uint32_t violation_check_interval_seconds;

    time_t created_at;
    time_t last_metric_collection;
    uint8_t is_active;
} SLAManager;

int slm_create_manager(const char *name);
int slm_delete_manager(uint32_t manager_id);

int slm_create_sla_definition(uint32_t manager_id, const char *sla_name,
                              const char *service_name, uint32_t owner_id);
int slm_delete_sla_definition(uint32_t manager_id, uint32_t sla_id);
int slm_get_sla_definitions(uint32_t manager_id, SLADefinition *definitions, uint32_t *count);

int slm_create_service_level(uint32_t manager_id, const char *level_name,
                             double availability, uint32_t max_response_time_ms,
                             uint32_t min_throughput, double max_error_rate);
int slm_delete_service_level(uint32_t manager_id, uint32_t level_id);
int slm_apply_service_level_to_sla(uint32_t manager_id, uint32_t sla_id, uint32_t level_id);

int slm_add_metric_to_sla(uint32_t manager_id, uint32_t sla_id, SLAMetricType metric_type,
                          const char *metric_name, SLAThresholdType threshold_type,
                          double min_val, double max_val, double warning_val);
int slm_remove_metric_from_sla(uint32_t manager_id, uint32_t sla_id, uint32_t metric_id);

int slm_record_metric_reading(uint32_t manager_id, uint32_t sla_id, SLAMetricType metric_type,
                              double metric_value);
int slm_get_metric_readings(uint32_t manager_id, SLAMetricReading *readings, uint32_t *count);

int slm_detect_sla_violation(uint32_t manager_id, uint32_t sla_id, SLAMetricType metric_type,
                             double expected_value, double actual_value,
                             ViolationSeverity severity);
int slm_acknowledge_violation(uint32_t manager_id, uint64_t violation_id);
int slm_get_violations(uint32_t manager_id, SLAViolation *violations, uint32_t *count);

int slm_add_remediation_action(uint32_t manager_id, uint64_t violation_id,
                               RemediationActionType action_type, const char *description);
int slm_complete_remediation_action(uint32_t manager_id, uint32_t action_id,
                                    uint8_t success, const char *result_details);
int slm_get_remediation_actions(uint32_t manager_id, RemediationAction *actions,
                                uint32_t *count);

int slm_check_sla_compliance(uint32_t manager_id, uint32_t sla_id);
int slm_calculate_compliance_percentage(uint32_t manager_id, uint32_t sla_id,
                                        double *compliance_percentage);
int slm_issue_credit(uint32_t manager_id, uint32_t sla_id, double credit_amount);
int slm_apply_penalty(uint32_t manager_id, uint32_t sla_id, double penalty_amount);

int slm_enable_auto_remediation(uint32_t manager_id);
int slm_disable_auto_remediation(uint32_t manager_id);
int slm_perform_auto_remediation(uint32_t manager_id, uint64_t violation_id);

uint64_t slm_get_total_sla_breaches(uint32_t manager_id);
uint64_t slm_get_total_violations_detected(uint32_t manager_id);
uint64_t slm_get_total_violations_remediated(uint32_t manager_id);
double slm_get_total_penalties(uint32_t manager_id);
double slm_get_total_credits(uint32_t manager_id);
double slm_get_average_remediation_time(uint32_t manager_id);
double slm_get_overall_compliance(uint32_t manager_id);

int slm_generate_sla_report(uint32_t manager_id, uint32_t sla_id, const char *output_path);
int slm_export_sla_metrics(uint32_t manager_id, const char *export_path);

SLAManager *slm_get_manager_info(uint32_t manager_id);

#endif
