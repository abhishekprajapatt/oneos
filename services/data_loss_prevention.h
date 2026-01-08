#ifndef DATA_LOSS_PREVENTION_H
#define DATA_LOSS_PREVENTION_H

#include <stdint.h>
#include <time.h>

#define MAX_DLP_MANAGERS 16
#define MAX_DLP_POLICIES 256
#define MAX_DLP_RULES 512
#define MAX_PROTECTED_RESOURCES 2048
#define MAX_DLP_INCIDENTS 1024
#define MAX_SENSITIVE_DATA_PATTERNS 256
#define MAX_ALLOWED_DESTINATIONS 512

typedef enum
{
    DLP_POLICY_TYPE_DATA_CLASSIFICATION = 0,
    DLP_POLICY_TYPE_ENDPOINT_PROTECTION = 1,
    DLP_POLICY_TYPE_NETWORK_PROTECTION = 2,
    DLP_POLICY_TYPE_CLOUD_PROTECTION = 3,
    DLP_POLICY_TYPE_EMAIL_PROTECTION = 4
} DLPPolicyType;

typedef enum
{
    DATA_CLASSIFICATION_PUBLIC = 0,
    DATA_CLASSIFICATION_INTERNAL = 1,
    DATA_CLASSIFICATION_CONFIDENTIAL = 2,
    DATA_CLASSIFICATION_SECRET = 3,
    DATA_CLASSIFICATION_RESTRICTED = 4
} DataClassification;

typedef enum
{
    DLP_ACTION_AUDIT = 0,
    DLP_ACTION_WARN = 1,
    DLP_ACTION_BLOCK = 2,
    DLP_ACTION_QUARANTINE = 3,
    DLP_ACTION_ENCRYPT = 4,
    DLP_ACTION_REDACT = 5
} DLPAction;

typedef enum
{
    DLP_RULE_TYPE_PATTERN_MATCHING = 0,
    DLP_RULE_TYPE_FILE_FINGERPRINTING = 1,
    DLP_RULE_TYPE_CONTENT_INSPECTION = 2,
    DLP_RULE_TYPE_METADATA_ANALYSIS = 3,
    DLP_RULE_TYPE_BEHAVIORAL = 4,
    DLP_RULE_TYPE_CONTEXT_AWARE = 5
} DLPRuleType;

typedef enum
{
    DLP_INCIDENT_STATUS_DETECTED = 0,
    DLP_INCIDENT_STATUS_INVESTIGATED = 1,
    DLP_INCIDENT_STATUS_RESOLVED = 2,
    DLP_INCIDENT_STATUS_FALSE_POSITIVE = 3,
    DLP_INCIDENT_STATUS_ESCALATED = 4
} DLPIncidentStatus;

typedef struct
{
    uint32_t pattern_id;
    char pattern_name[127];
    char pattern_regex[511];
    DataClassification data_class;
    uint32_t match_count;
    uint32_t false_positive_count;
    uint8_t is_enabled;
    uint16_t confidence_level;
} SensitiveDataPattern;

typedef struct
{
    uint32_t rule_id;
    char rule_name[127];
    DLPRuleType rule_type;
    char rule_expression[255];
    DLPAction action_on_match;
    uint32_t pattern_id;
    uint32_t trigger_count;
    uint8_t is_enabled;
    uint8_t requires_escalation;
    uint16_t severity_score;
} DLPRule;

typedef struct
{
    uint32_t policy_id;
    char policy_name[127];
    DLPPolicyType policy_type;
    DataClassification min_classification;
    uint32_t rule_count;
    uint32_t incident_count;
    uint8_t is_enabled;
    uint8_t is_global_policy;
    uint32_t user_scope;
    time_t created_at;
    time_t last_modified;
    char description[255];
} DLPPolicy;

typedef struct
{
    uint32_t resource_id;
    char resource_name[127];
    char resource_path[255];
    DataClassification classification;
    uint64_t file_hash;
    uint32_t policy_id;
    uint32_t owner_id;
    uint32_t access_count;
    time_t created_at;
    time_t last_accessed;
    time_t last_modified;
    uint8_t is_protected;
} ProtectedResource;

typedef struct
{
    uint32_t destination_id;
    char destination_name[127];
    char destination_address[255];
    DataClassification min_data_class;
    uint8_t is_trusted;
    uint8_t requires_encryption;
    uint8_t requires_approval;
    uint32_t allowed_transfer_rate_kbps;
    time_t last_access;
} AllowedDestination;

typedef struct
{
    uint64_t incident_id;
    char incident_name[127];
    uint32_t rule_id;
    uint32_t policy_id;
    DLPIncidentStatus status;
    uint32_t source_user_id;
    char source_location[255];
    DataClassification data_class;
    uint32_t affected_record_count;
    uint64_t data_volume_bytes;
    DLPAction action_taken;
    time_t detected_at;
    time_t resolved_at;
    char remediation_notes[255];
    uint32_t investigation_duration_minutes;
} DLPIncident;

typedef struct
{
    uint32_t manager_id;
    char name[63];
    uint32_t policy_count;
    uint32_t rule_count;
    uint32_t pattern_count;
    uint32_t resource_count;
    uint32_t destination_count;
    uint32_t incident_count;
    uint64_t total_incidents;
    uint64_t total_violations;
    uint64_t total_blocked;
    uint64_t total_quarantined;
    uint64_t total_encrypted;
    uint32_t policy_version;
    time_t last_policy_update;
    uint8_t auto_remediation_enabled;
    uint8_t is_active;
    DLPPolicy policies[MAX_DLP_POLICIES];
    DLPRule rules[MAX_DLP_RULES];
    SensitiveDataPattern patterns[MAX_SENSITIVE_DATA_PATTERNS];
    ProtectedResource resources[MAX_PROTECTED_RESOURCES];
    AllowedDestination destinations[MAX_ALLOWED_DESTINATIONS];
    DLPIncident incidents[MAX_DLP_INCIDENTS];
} DataLossPrevention;

int dlp_create_manager(const char *name);
int dlp_delete_manager(uint32_t dlp_id);
int dlp_add_policy(uint32_t dlp_id, const char *policy_name, DLPPolicyType policy_type,
                   DataClassification min_class);
int dlp_remove_policy(uint32_t dlp_id, uint32_t policy_id);
int dlp_enable_policy(uint32_t dlp_id, uint32_t policy_id);
int dlp_disable_policy(uint32_t dlp_id, uint32_t policy_id);
int dlp_add_sensitive_pattern(uint32_t dlp_id, const char *pattern_name,
                              const char *pattern_regex, DataClassification data_class);
int dlp_add_dlp_rule(uint32_t dlp_id, const char *rule_name, DLPRuleType rule_type,
                     DLPAction action, uint32_t pattern_id);
int dlp_remove_dlp_rule(uint32_t dlp_id, uint32_t rule_id);
int dlp_add_protected_resource(uint32_t dlp_id, const char *resource_name,
                               const char *resource_path, DataClassification classification);
int dlp_classify_resource(uint32_t dlp_id, uint32_t resource_id, DataClassification classification);
int dlp_add_allowed_destination(uint32_t dlp_id, const char *dest_name,
                                const char *dest_address, DataClassification min_class);
int dlp_remove_allowed_destination(uint32_t dlp_id, uint32_t destination_id);
int dlp_scan_for_violations(uint32_t dlp_id);
int dlp_scan_resource(uint32_t dlp_id, uint32_t resource_id);
int dlp_detect_violation(uint32_t dlp_id, uint32_t rule_id, uint32_t source_user_id,
                         const char *source_location, DataClassification data_class,
                         uint32_t record_count, uint64_t data_volume);
int dlp_block_data_transfer(uint32_t dlp_id, uint64_t incident_id);
int dlp_quarantine_data(uint32_t dlp_id, uint64_t incident_id, const char *quarantine_path);
int dlp_encrypt_data(uint32_t dlp_id, uint64_t incident_id);
int dlp_redact_sensitive_content(uint32_t dlp_id, uint32_t resource_id);
int dlp_investigate_incident(uint32_t dlp_id, uint64_t incident_id);
int dlp_resolve_incident(uint32_t dlp_id, uint64_t incident_id, DLPIncidentStatus status);
int dlp_get_active_incidents(uint32_t dlp_id, DLPIncident *incidents, uint32_t *count);
int dlp_get_incident_details(uint32_t dlp_id, uint64_t incident_id, DLPIncident *incident);
uint64_t dlp_get_total_violations(uint32_t dlp_id);
uint64_t dlp_get_total_blocked_transfers(uint32_t dlp_id);
uint64_t dlp_get_total_data_encrypted(uint32_t dlp_id);
int dlp_generate_dlp_report(uint32_t dlp_id, const char *output_path);
int dlp_export_dlp_config(uint32_t dlp_id, const char *export_path);
int dlp_enable_auto_remediation(uint32_t dlp_id);
int dlp_disable_auto_remediation(uint32_t dlp_id);
DataLossPrevention *dlp_get_manager_info(uint32_t dlp_id);

#endif // DATA_LOSS_PREVENTION_H
