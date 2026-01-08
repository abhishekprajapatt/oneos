#ifndef SECURITY_CORE_H
#define SECURITY_CORE_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_SECURITY_POLICIES 256
#define MAX_ACCESS_RULES 1024
#define MAX_AUDIT_LOGS 8192
#define MAX_ENCRYPTION_KEYS 512
#define MAX_USERS 128

typedef enum
{
    PERMISSION_NONE = 0,
    PERMISSION_READ = 1,
    PERMISSION_WRITE = 2,
    PERMISSION_EXECUTE = 4,
    PERMISSION_DELETE = 8,
    PERMISSION_ADMIN = 16
} Permission;

typedef enum
{
    RULE_ALLOW = 0,
    RULE_DENY = 1,
    RULE_AUDIT = 2
} RuleType;

typedef enum
{
    AUDIT_ACTION_READ = 0,
    AUDIT_ACTION_WRITE = 1,
    AUDIT_ACTION_EXECUTE = 2,
    AUDIT_ACTION_DELETE = 3,
    AUDIT_ACTION_PERMISSION_CHANGE = 4,
    AUDIT_ACTION_FAILED_ACCESS = 5
} AuditAction;

typedef enum
{
    ENCRYPT_AES128 = 0,
    ENCRYPT_AES256 = 1,
    ENCRYPT_SHA256 = 2,
    ENCRYPT_RSA2048 = 3
} EncryptionMethod;

typedef struct
{
    uint32_t user_id;
    char username[64];
    char password_hash[128];
    uint32_t permissions;
    bool is_active;
    bool is_admin;
    uint32_t failed_login_attempts;
    time_t last_login;
    time_t created_time;
} User;

typedef struct
{
    uint32_t rule_id;
    uint32_t subject_id;
    uint32_t object_id;
    RuleType rule_type;
    uint32_t permissions;
    bool is_active;
    time_t created_time;
    time_t expires_time;
} AccessRule;

typedef struct
{
    uint32_t policy_id;
    char name[64];
    AccessRule rules[MAX_ACCESS_RULES];
    uint32_t rule_count;
    bool is_enabled;
    uint32_t enforcement_level;
} SecurityPolicy;

typedef struct
{
    uint32_t log_id;
    AuditAction action;
    uint32_t user_id;
    uint32_t subject_id;
    uint32_t object_id;
    bool success;
    time_t timestamp;
    char details[256];
} AuditLog;

typedef struct
{
    uint32_t key_id;
    EncryptionMethod method;
    uint8_t key_data[256];
    uint32_t key_size;
    time_t creation_time;
    time_t expiration_time;
    bool is_active;
} EncryptionKey;

typedef struct
{
    User users[MAX_USERS];
    uint32_t user_count;
    SecurityPolicy policies[MAX_SECURITY_POLICIES];
    uint32_t policy_count;
    AuditLog audit_logs[MAX_AUDIT_LOGS];
    uint32_t audit_log_count;
    EncryptionKey keys[MAX_ENCRYPTION_KEYS];
    uint32_t key_count;
    bool selinux_enabled;
    uint32_t failed_access_threshold;
    uint32_t lockout_duration_sec;
    uint64_t total_access_denied;
    uint64_t total_access_granted;
} SecurityCore;

SecurityCore *security_init(void);
void security_cleanup(SecurityCore *core);

uint32_t security_create_user(SecurityCore *core, const char *username, const char *password);
int security_delete_user(SecurityCore *core, uint32_t user_id);
int security_authenticate_user(SecurityCore *core, uint32_t user_id, const char *password);
int security_set_user_permissions(SecurityCore *core, uint32_t user_id, uint32_t permissions);
User *security_get_user(SecurityCore *core, uint32_t user_id);

uint32_t security_create_policy(SecurityCore *core, const char *name, uint32_t enforcement_level);
int security_add_access_rule(SecurityCore *core, uint32_t policy_id, uint32_t subject_id,
                             uint32_t object_id, RuleType rule_type, uint32_t permissions);
int security_remove_access_rule(SecurityCore *core, uint32_t policy_id, uint32_t rule_id);
int security_enable_policy(SecurityCore *core, uint32_t policy_id, bool enable);
SecurityPolicy *security_get_policy(SecurityCore *core, uint32_t policy_id);

int security_check_access(SecurityCore *core, uint32_t user_id, uint32_t object_id,
                          Permission required_permission);
int security_audit_action(SecurityCore *core, uint32_t user_id, uint32_t subject_id,
                          uint32_t object_id, AuditAction action, bool success, const char *details);

uint32_t security_create_encryption_key(SecurityCore *core, EncryptionMethod method, uint32_t size);
int security_delete_encryption_key(SecurityCore *core, uint32_t key_id);
EncryptionKey *security_get_encryption_key(SecurityCore *core, uint32_t key_id);

int security_enable_selinux(SecurityCore *core, bool enable);
int security_set_failed_access_threshold(SecurityCore *core, uint32_t threshold);
int security_set_lockout_duration(SecurityCore *core, uint32_t seconds);

uint32_t security_get_audit_log_count(SecurityCore *core);
AuditLog *security_get_audit_log(SecurityCore *core, uint32_t log_id);
uint64_t security_get_total_access_denied(SecurityCore *core);
uint64_t security_get_total_access_granted(SecurityCore *core);

#endif
