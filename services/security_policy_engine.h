#ifndef SECURITY_POLICY_ENGINE_H
#define SECURITY_POLICY_ENGINE_H

#include <stdint.h>
#include <time.h>

#define MAX_POLICY_ENGINES 16
#define MAX_SECURITY_POLICIES 512
#define MAX_POLICY_RULES 2048
#define MAX_USER_ROLES 256
#define MAX_PERMISSIONS 1024
#define MAX_POLICY_VIOLATIONS 2048
#define MAX_SECURITY_GROUPS 512

typedef enum
{
    POLICY_TYPE_ACCESS_CONTROL = 0,
    POLICY_TYPE_PASSWORD_POLICY = 1,
    POLICY_TYPE_ENCRYPTION_POLICY = 2,
    POLICY_TYPE_AUDIT_POLICY = 3,
    POLICY_TYPE_NETWORK_POLICY = 4,
    POLICY_TYPE_ENDPOINT_POLICY = 5
} PolicyType;

typedef enum
{
    ROLE_ADMIN = 0,
    ROLE_USER = 1,
    ROLE_GUEST = 2,
    ROLE_SERVICE = 3,
    ROLE_SYSTEM = 4
} UserRole;

typedef enum
{
    PERMISSION_READ = 1,
    PERMISSION_WRITE = 2,
    PERMISSION_EXECUTE = 4,
    PERMISSION_DELETE = 8,
    PERMISSION_MANAGE = 16,
    PERMISSION_ADMIN = 32
} Permission;

typedef enum
{
    POLICY_STATE_DRAFT = 0,
    POLICY_STATE_ACTIVE = 1,
    POLICY_STATE_DEPRECATED = 2,
    POLICY_STATE_ENFORCED = 3,
    POLICY_STATE_SUSPENDED = 4
} PolicyState;

typedef enum
{
    VIOLATION_SEVERITY_LOW = 0,
    VIOLATION_SEVERITY_MEDIUM = 1,
    VIOLATION_SEVERITY_HIGH = 2,
    VIOLATION_SEVERITY_CRITICAL = 3
} ViolationSeverity;

typedef struct
{
    uint32_t permission_id;
    char permission_name[63];
    uint32_t permission_flags;
    char resource_type[63];
    uint8_t is_inherited;
} ResourcePermission;

typedef struct
{
    uint32_t role_id;
    char role_name[63];
    uint32_t permission_count;
    uint64_t permissions_bitmask;
    uint32_t user_count;
    uint8_t is_system_role;
    time_t created_at;
    ResourcePermission permissions[MAX_PERMISSIONS];
} UserRoleDefinition;

typedef struct
{
    uint32_t policy_id;
    char policy_name[127];
    PolicyType policy_type;
    PolicyState state;
    char description[255];
    uint32_t rule_count;
    uint32_t affected_users;
    uint32_t affected_resources;
    uint32_t violation_count;
    uint8_t is_mandatory;
    time_t created_at;
    time_t modified_at;
    time_t enforced_at;
    uint32_t policy_version;
} SecurityPolicy;

typedef struct
{
    uint32_t group_id;
    char group_name[63];
    char group_description[255];
    uint32_t member_count;
    uint32_t role_id;
    time_t created_at;
    uint8_t is_dynamic;
} SecurityGroup;

typedef struct
{
    uint32_t rule_id;
    uint32_t policy_id;
    char rule_name[127];
    char rule_condition[511];
    uint32_t resource_id;
    uint32_t user_role_id;
    uint32_t action_mask;
    uint8_t is_enabled;
    uint16_t rule_priority;
    ViolationSeverity violation_severity;
} PolicyRule;

typedef struct
{
    uint64_t violation_id;
    uint32_t policy_id;
    uint32_t rule_id;
    uint32_t user_id;
    uint32_t resource_id;
    ViolationSeverity severity;
    char violation_description[255];
    time_t detected_at;
    time_t resolved_at;
    uint8_t is_remediated;
    uint8_t requires_audit;
} PolicyViolation;

typedef struct
{
    uint32_t engine_id;
    char name[63];
    uint32_t policy_count;
    uint32_t role_count;
    uint32_t group_count;
    uint32_t violation_count;
    uint64_t total_violations;
    uint64_t violations_remediated;
    uint64_t violations_escalated;
    uint32_t enforcement_level;
    uint8_t is_active;
    time_t last_policy_sync;
    SecurityPolicy policies[MAX_SECURITY_POLICIES];
    UserRoleDefinition roles[MAX_USER_ROLES];
    SecurityGroup groups[MAX_SECURITY_GROUPS];
    PolicyRule rules[MAX_POLICY_RULES];
    PolicyViolation violations[MAX_POLICY_VIOLATIONS];
} SecurityPolicyEngine;

int spe_create_engine(const char *name);
int spe_delete_engine(uint32_t engine_id);
int spe_add_security_policy(uint32_t engine_id, const char *policy_name,
                            PolicyType policy_type, const char *description);
int spe_remove_security_policy(uint32_t engine_id, uint32_t policy_id);
int spe_activate_policy(uint32_t engine_id, uint32_t policy_id);
int spe_deactivate_policy(uint32_t engine_id, uint32_t policy_id);
int spe_enforce_policy(uint32_t engine_id, uint32_t policy_id);
int spe_create_user_role(uint32_t engine_id, const char *role_name);
int spe_delete_user_role(uint32_t engine_id, uint32_t role_id);
int spe_grant_permission(uint32_t engine_id, uint32_t role_id,
                         const char *permission_name, uint32_t permission_flags);
int spe_revoke_permission(uint32_t engine_id, uint32_t role_id, uint32_t permission_id);
int spe_add_policy_rule(uint32_t engine_id, uint32_t policy_id,
                        const char *rule_name, const char *condition);
int spe_remove_policy_rule(uint32_t engine_id, uint32_t rule_id);
int spe_create_security_group(uint32_t engine_id, const char *group_name, uint32_t role_id);
int spe_delete_security_group(uint32_t engine_id, uint32_t group_id);
int spe_check_policy_compliance(uint32_t engine_id, uint32_t user_id, uint32_t resource_id);
int spe_detect_policy_violation(uint32_t engine_id, uint32_t policy_id, uint32_t rule_id,
                                uint32_t user_id, uint32_t resource_id,
                                const char *violation_description);
int spe_remediate_violation(uint32_t engine_id, uint64_t violation_id);
int spe_escalate_violation(uint32_t engine_id, uint64_t violation_id);
int spe_get_policy_violations(uint32_t engine_id, PolicyViolation *violations, uint32_t *count);
int spe_get_user_permissions(uint32_t engine_id, uint32_t user_id, uint32_t *permissions);
int spe_audit_policy_changes(uint32_t engine_id);
int spe_sync_policies(uint32_t engine_id);
uint64_t spe_get_total_violations(uint32_t engine_id);
uint64_t spe_get_violations_remediated(uint32_t engine_id);
int spe_generate_policy_report(uint32_t engine_id, const char *output_path);
int spe_export_policy_config(uint32_t engine_id, const char *export_path);
SecurityPolicyEngine *spe_get_engine_info(uint32_t engine_id);

#endif // SECURITY_POLICY_ENGINE_H
