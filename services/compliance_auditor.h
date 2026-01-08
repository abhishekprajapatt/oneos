#ifndef ONEOS_COMPLIANCE_AUDITOR_H
#define ONEOS_COMPLIANCE_AUDITOR_H

#include <time.h>
#include <stdint.h>

#define MAX_COMPLIANCE_AUDITORS 16
#define MAX_COMPLIANCE_FRAMEWORKS 64
#define MAX_COMPLIANCE_CONTROLS 512
#define MAX_AUDIT_LOGS 4096
#define MAX_COMPLIANCE_VIOLATIONS 1024
#define MAX_REMEDIATION_ITEMS 2048
#define MAX_AUDIT_TRAILS 8192

typedef enum
{
    FRAMEWORK_HIPAA = 0,
    FRAMEWORK_PCI_DSS = 1,
    FRAMEWORK_SOC2 = 2,
    FRAMEWORK_ISO_27001 = 3,
    FRAMEWORK_GDPR = 4,
    FRAMEWORK_NIST = 5,
    FRAMEWORK_CIS = 6
} ComplianceFramework;

typedef enum
{
    CONTROL_STATUS_NOT_ASSESSED = 0,
    CONTROL_STATUS_COMPLIANT = 1,
    CONTROL_STATUS_NON_COMPLIANT = 2,
    CONTROL_STATUS_PARTIAL_COMPLIANT = 3,
    CONTROL_STATUS_NOT_APPLICABLE = 4
} ControlStatus;

typedef enum
{
    VIOLATION_SEVERITY_LOW = 0,
    VIOLATION_SEVERITY_MEDIUM = 1,
    VIOLATION_SEVERITY_HIGH = 2,
    VIOLATION_SEVERITY_CRITICAL = 3
} ViolationSeverity;

typedef enum
{
    AUDIT_ACTION_ACCESS = 0,
    AUDIT_ACTION_MODIFY = 1,
    AUDIT_ACTION_DELETE = 2,
    AUDIT_ACTION_CREATE = 3,
    AUDIT_ACTION_AUTHENTICATE = 4,
    AUDIT_ACTION_AUTHORIZE = 5,
    AUDIT_ACTION_CONFIGURATION_CHANGE = 6,
    AUDIT_ACTION_SECURITY_EVENT = 7
} AuditAction;

typedef enum
{
    REMEDIATION_STATUS_PENDING = 0,
    REMEDIATION_STATUS_IN_PROGRESS = 1,
    REMEDIATION_STATUS_COMPLETED = 2,
    REMEDIATION_STATUS_FAILED = 3,
    REMEDIATION_STATUS_VERIFIED = 4
} RemediationStatus;

typedef struct
{
    uint32_t control_id;
    char control_name[128];
    ComplianceFramework framework;
    char control_description[256];
    ControlStatus status;
    char requirements[512];
    time_t last_assessed;
    uint8_t is_critical;
    uint32_t assessment_count;
} ComplianceControl;

typedef struct
{
    uint64_t violation_id;
    char violation_description[256];
    ComplianceFramework framework;
    uint32_t control_id;
    ViolationSeverity severity;
    time_t detected_at;
    time_t remediated_at;
    uint32_t days_open;
    uint8_t is_resolved;
    char remediation_notes[256];
} ComplianceViolation;

typedef struct
{
    uint64_t audit_id;
    uint32_t actor_id;
    AuditAction action;
    char resource_path[256];
    char ip_address[45];
    time_t action_timestamp;
    uint8_t was_successful;
    char details[512];
} AuditTrail;

typedef struct
{
    uint32_t remediation_id;
    uint64_t violation_id;
    ComplianceFramework framework;
    char remediation_plan[512];
    RemediationStatus status;
    time_t created_at;
    time_t target_completion;
    time_t actual_completion;
    uint32_t responsible_team_id;
    char responsible_name[64];
} RemediationItem;

typedef struct
{
    uint32_t framework_id;
    ComplianceFramework framework_type;
    char framework_name[64];
    uint32_t total_controls;
    uint32_t compliant_controls;
    uint32_t non_compliant_controls;
    uint32_t partial_compliant_controls;
    uint32_t not_assessed_controls;
    uint8_t is_certified;
    time_t last_audit;
    time_t certification_expiry;
    double compliance_score;
} ComplianceFrameworkInfo;

typedef struct
{
    uint32_t auditor_id;
    char name[64];
    ComplianceFramework frameworks[MAX_COMPLIANCE_FRAMEWORKS];
    uint32_t framework_count;

    uint32_t control_count;
    ComplianceControl controls[MAX_COMPLIANCE_CONTROLS];

    uint32_t violation_count;
    ComplianceViolation violations[MAX_COMPLIANCE_VIOLATIONS];

    uint32_t audit_trail_count;
    AuditTrail audit_trails[MAX_AUDIT_TRAILS];

    uint32_t remediation_count;
    RemediationItem remediation_items[MAX_REMEDIATION_ITEMS];

    ComplianceFrameworkInfo framework_info[MAX_COMPLIANCE_FRAMEWORKS];

    uint64_t total_violations_detected;
    uint64_t total_violations_resolved;
    uint64_t total_audit_events;
    double overall_compliance_score;

    uint8_t continuous_monitoring_enabled;
    time_t created_at;
    time_t last_scan;
    uint8_t is_active;
} ComplianceAuditor;

int ca_create_auditor(const char *name);
int ca_delete_auditor(uint32_t auditor_id);

int ca_add_framework(uint32_t auditor_id, ComplianceFramework framework);
int ca_remove_framework(uint32_t auditor_id, ComplianceFramework framework);

int ca_add_control(uint32_t auditor_id, const char *control_name, ComplianceFramework framework,
                   const char *control_description, uint8_t is_critical);
int ca_assess_control(uint32_t auditor_id, uint32_t control_id, ControlStatus status);
int ca_get_controls_by_framework(uint32_t auditor_id, ComplianceFramework framework,
                                 ComplianceControl *controls, uint32_t *count);

int ca_detect_violation(uint32_t auditor_id, const char *violation_desc,
                        ComplianceFramework framework, uint32_t control_id,
                        ViolationSeverity severity);
int ca_resolve_violation(uint32_t auditor_id, uint64_t violation_id,
                         const char *remediation_notes);
int ca_get_violations(uint32_t auditor_id, ComplianceViolation *violations, uint32_t *count);

int ca_create_remediation_item(uint32_t auditor_id, uint64_t violation_id,
                               const char *remediation_plan, uint32_t responsible_team_id);
int ca_update_remediation_status(uint32_t auditor_id, uint32_t remediation_id,
                                 RemediationStatus status);
int ca_get_remediation_items(uint32_t auditor_id, RemediationItem *items, uint32_t *count);

int ca_log_audit_event(uint32_t auditor_id, uint32_t actor_id, AuditAction action,
                       const char *resource_path, const char *ip_address, uint8_t success,
                       const char *details);
int ca_get_audit_trails(uint32_t auditor_id, AuditTrail *trails, uint32_t *count);
int ca_get_audit_events_by_action(uint32_t auditor_id, AuditAction action,
                                  AuditTrail *trails, uint32_t *count);

int ca_calculate_compliance_score(uint32_t auditor_id, ComplianceFramework framework);
int ca_generate_compliance_report(uint32_t auditor_id, ComplianceFramework framework,
                                  const char *output_path);

int ca_enable_continuous_monitoring(uint32_t auditor_id);
int ca_disable_continuous_monitoring(uint32_t auditor_id);

int ca_export_audit_log(uint32_t auditor_id, const char *export_path);
int ca_export_compliance_certification(uint32_t auditor_id, ComplianceFramework framework,
                                       const char *cert_path);

uint64_t ca_get_total_violations_detected(uint32_t auditor_id);
uint64_t ca_get_total_violations_resolved(uint32_t auditor_id);
uint64_t ca_get_total_audit_events(uint32_t auditor_id);
double ca_get_overall_compliance_score(uint32_t auditor_id);
double ca_get_framework_compliance_score(uint32_t auditor_id, ComplianceFramework framework);

ComplianceAuditor *ca_get_auditor_info(uint32_t auditor_id);

#endif
