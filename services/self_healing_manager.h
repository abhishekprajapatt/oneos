#ifndef ONEOS_SELF_HEALING_MANAGER_H
#define ONEOS_SELF_HEALING_MANAGER_H

#include <stdint.h>
#include <time.h>

#define MAX_HEALING_MANAGERS 16
#define MAX_HEALTH_CHECKS 256
#define MAX_RECOVERY_ACTIONS 512
#define MAX_FAULT_PATTERNS 128
#define MAX_SERVICE_DEPENDENCIES 256

typedef enum
{
    FAULT_TYPE_HARDWARE = 0,
    FAULT_TYPE_SOFTWARE = 1,
    FAULT_TYPE_NETWORK = 2,
    FAULT_TYPE_SERVICE = 3,
    FAULT_TYPE_RESOURCE = 4,
    FAULT_TYPE_PERFORMANCE = 5
} FaultType;

typedef enum
{
    SEVERITY_LEVEL_INFO = 0,
    SEVERITY_LEVEL_WARNING = 1,
    SEVERITY_LEVEL_ERROR = 2,
    SEVERITY_LEVEL_CRITICAL = 3,
    SEVERITY_LEVEL_FATAL = 4
} SeverityLevel;

typedef enum
{
    HEALTH_CHECK_TYPE_PING = 0,
    HEALTH_CHECK_TYPE_HTTP = 1,
    HEALTH_CHECK_TYPE_TCP = 2,
    HEALTH_CHECK_TYPE_MEMORY = 3,
    HEALTH_CHECK_TYPE_CPU = 4,
    HEALTH_CHECK_TYPE_DISK = 5,
    HEALTH_CHECK_TYPE_PROCESS = 6,
    HEALTH_CHECK_TYPE_DATABASE = 7
} HealthCheckType;

typedef enum
{
    RECOVERY_ACTION_RESTART_SERVICE = 0,
    RECOVERY_ACTION_RESTART_PROCESS = 1,
    RECOVERY_ACTION_FAILOVER = 2,
    RECOVERY_ACTION_ROLLBACK = 3,
    RECOVERY_ACTION_ISOLATE_SERVICE = 4,
    RECOVERY_ACTION_CLEAR_CACHE = 5,
    RECOVERY_ACTION_RESET_CONNECTION = 6,
    RECOVERY_ACTION_SCALE_UP = 7
} RecoveryAction;

typedef enum
{
    HEALTH_STATUS_HEALTHY = 0,
    HEALTH_STATUS_DEGRADED = 1,
    HEALTH_STATUS_UNHEALTHY = 2,
    HEALTH_STATUS_RECOVERING = 3,
    HEALTH_STATUS_UNKNOWN = 4
} HealthStatus;

typedef struct
{
    uint32_t check_id;
    char check_name[64];
    char target_service[64];
    HealthCheckType check_type;
    uint32_t interval_seconds;
    uint32_t timeout_seconds;
    uint32_t failure_threshold;
    uint32_t consecutive_failures;
    time_t last_check_time;
    HealthStatus last_check_status;
    uint8_t is_enabled;
} HealthCheck;

typedef struct
{
    uint32_t action_id;
    char action_name[64];
    RecoveryAction action_type;
    char target_service[64];
    char action_parameters[256];
    uint32_t priority;
    uint32_t timeout_seconds;
    time_t last_execution_time;
    uint32_t execution_count;
    uint32_t success_count;
    uint8_t is_enabled;
} RecoveryActionPolicy;

typedef struct
{
    uint32_t incident_id;
    FaultType fault_type;
    SeverityLevel severity;
    char description[256];
    time_t detection_time;
    time_t resolution_time;
    char affected_component[64];
    uint32_t impact_score;
    RecoveryAction actions_taken;
    uint8_t auto_resolved;
} FaultIncident;

typedef struct
{
    uint32_t pattern_id;
    char pattern_name[64];
    FaultType fault_type;
    char symptom_description[256];
    uint32_t detection_confidence;
    RecoveryAction recommended_action;
    uint32_t occurrences;
    time_t last_detected;
} FaultPattern;

typedef struct
{
    uint32_t dependency_id;
    char dependent_service[64];
    char dependency_service[64];
    uint8_t is_critical;
    uint32_t timeout_seconds;
} ServiceDependency;

typedef struct
{
    uint32_t manager_id;
    char name[64];

    uint32_t health_check_count;
    HealthCheck health_checks[256];

    uint32_t recovery_action_count;
    RecoveryActionPolicy recovery_actions[512];

    uint32_t fault_incident_count;
    FaultIncident fault_incidents[128];

    uint32_t fault_pattern_count;
    FaultPattern fault_patterns[128];

    uint32_t service_dependency_count;
    ServiceDependency service_dependencies[256];

    uint32_t services_monitored;
    uint32_t incidents_detected;
    uint32_t incidents_auto_resolved;
    uint32_t recovery_actions_executed;
    double system_health_score;

    time_t created_at;
    time_t last_health_assessment;
    uint8_t is_active;
} SelfHealingManager;

int shm_create_manager(const char *name);
int shm_delete_manager(uint32_t manager_id);

int shm_create_health_check(uint32_t manager_id, const char *check_name,
                            const char *target_service, HealthCheckType check_type,
                            uint32_t interval_seconds);
int shm_enable_health_check(uint32_t manager_id, uint32_t check_id);
int shm_disable_health_check(uint32_t manager_id, uint32_t check_id);
int shm_delete_health_check(uint32_t manager_id, uint32_t check_id);

int shm_create_recovery_action(uint32_t manager_id, const char *action_name,
                               RecoveryAction action_type, const char *target_service,
                               uint32_t priority);
int shm_enable_recovery_action(uint32_t manager_id, uint32_t action_id);
int shm_disable_recovery_action(uint32_t manager_id, uint32_t action_id);
int shm_delete_recovery_action(uint32_t manager_id, uint32_t action_id);

int shm_register_service_dependency(uint32_t manager_id, const char *dependent,
                                    const char *dependency, uint8_t is_critical);
int shm_unregister_service_dependency(uint32_t manager_id, uint32_t dependency_id);

int shm_detect_fault(uint32_t manager_id, FaultType fault_type, SeverityLevel severity,
                     const char *description, const char *affected_component);
int shm_execute_recovery_action(uint32_t manager_id, uint32_t incident_id,
                                RecoveryAction action);
int shm_resolve_incident(uint32_t manager_id, uint32_t incident_id);

int shm_register_fault_pattern(uint32_t manager_id, const char *pattern_name,
                               FaultType fault_type, const char *symptom_description,
                               RecoveryAction recommended_action);
int shm_match_fault_pattern(uint32_t manager_id, FaultType fault_type,
                            const char *symptoms, uint32_t *pattern_id);

int shm_perform_health_assessment(uint32_t manager_id);
double shm_get_system_health_score(uint32_t manager_id);
HealthStatus shm_get_service_health(uint32_t manager_id, const char *service_name);

int shm_get_incidents(uint32_t manager_id, FaultIncident *incidents, uint32_t *count);
int shm_get_recovery_actions_for_service(uint32_t manager_id, const char *service_name,
                                         RecoveryActionPolicy *actions, uint32_t *count);

SelfHealingManager *shm_get_manager_info(uint32_t manager_id);

#endif
