#include "security_core.h"
#include <stdlib.h>
#include <string.h>

SecurityCore *security_init(void)
{
    SecurityCore *core = (SecurityCore *)malloc(sizeof(SecurityCore));
    if (!core)
        return NULL;

    memset(core, 0, sizeof(SecurityCore));
    core->user_count = 0;
    core->policy_count = 0;
    core->audit_log_count = 0;
    core->key_count = 0;
    core->selinux_enabled = false;
    core->failed_access_threshold = 5;
    core->lockout_duration_sec = 900;
    core->total_access_denied = 0;
    core->total_access_granted = 0;

    return core;
}

void security_cleanup(SecurityCore *core)
{
    if (!core)
        return;
    memset(core, 0, sizeof(SecurityCore));
    free(core);
}

uint32_t security_create_user(SecurityCore *core, const char *username, const char *password)
{
    if (!core || core->user_count >= MAX_USERS || !username || !password)
        return 0;

    for (uint32_t i = 0; i < core->user_count; i++)
    {
        if (strcmp(core->users[i].username, username) == 0)
            return 0;
    }

    User *user = &core->users[core->user_count];
    user->user_id = core->user_count + 1;
    strncpy(user->username, username, 63);
    user->username[63] = '\0';
    strncpy(user->password_hash, password, 127);
    user->password_hash[127] = '\0';
    user->permissions = PERMISSION_READ | PERMISSION_WRITE;
    user->is_active = true;
    user->is_admin = false;
    user->failed_login_attempts = 0;
    user->last_login = 0;
    user->created_time = time(NULL);

    core->user_count++;
    return user->user_id;
}

int security_delete_user(SecurityCore *core, uint32_t user_id)
{
    if (!core)
        return -1;

    for (uint32_t i = 0; i < core->user_count; i++)
    {
        if (core->users[i].user_id == user_id)
        {
            memmove(&core->users[i], &core->users[i + 1],
                    (core->user_count - i - 1) * sizeof(User));
            core->user_count--;
            return 0;
        }
    }
    return -1;
}

int security_authenticate_user(SecurityCore *core, uint32_t user_id, const char *password)
{
    if (!core || !password)
        return -1;

    User *user = security_get_user(core, user_id);
    if (!user || !user->is_active)
        return -1;

    if (strcmp(user->password_hash, password) != 0)
    {
        user->failed_login_attempts++;
        if (user->failed_login_attempts >= core->failed_access_threshold)
        {
            user->is_active = false;
        }
        return -1;
    }

    user->failed_login_attempts = 0;
    user->last_login = time(NULL);
    core->total_access_granted++;
    return 0;
}

int security_set_user_permissions(SecurityCore *core, uint32_t user_id, uint32_t permissions)
{
    if (!core)
        return -1;

    User *user = security_get_user(core, user_id);
    if (!user)
        return -1;

    user->permissions = permissions;
    return 0;
}

User *security_get_user(SecurityCore *core, uint32_t user_id)
{
    if (!core)
        return NULL;

    for (uint32_t i = 0; i < core->user_count; i++)
    {
        if (core->users[i].user_id == user_id)
        {
            return &core->users[i];
        }
    }
    return NULL;
}

uint32_t security_create_policy(SecurityCore *core, const char *name, uint32_t enforcement_level)
{
    if (!core || core->policy_count >= MAX_SECURITY_POLICIES || !name)
        return 0;

    SecurityPolicy *policy = &core->policies[core->policy_count];
    policy->policy_id = core->policy_count + 1;
    strncpy(policy->name, name, 63);
    policy->name[63] = '\0';
    policy->rule_count = 0;
    policy->is_enabled = true;
    policy->enforcement_level = enforcement_level;

    core->policy_count++;
    return policy->policy_id;
}

int security_add_access_rule(SecurityCore *core, uint32_t policy_id, uint32_t subject_id,
                             uint32_t object_id, RuleType rule_type, uint32_t permissions)
{
    if (!core)
        return -1;

    SecurityPolicy *policy = security_get_policy(core, policy_id);
    if (!policy || policy->rule_count >= MAX_ACCESS_RULES)
        return -1;

    AccessRule *rule = &policy->rules[policy->rule_count];
    rule->rule_id = policy->rule_count + 1;
    rule->subject_id = subject_id;
    rule->object_id = object_id;
    rule->rule_type = rule_type;
    rule->permissions = permissions;
    rule->is_active = true;
    rule->created_time = time(NULL);
    rule->expires_time = 0;

    policy->rule_count++;
    return 0;
}

int security_remove_access_rule(SecurityCore *core, uint32_t policy_id, uint32_t rule_id)
{
    if (!core)
        return -1;

    SecurityPolicy *policy = security_get_policy(core, policy_id);
    if (!policy)
        return -1;

    for (uint32_t i = 0; i < policy->rule_count; i++)
    {
        if (policy->rules[i].rule_id == rule_id)
        {
            memmove(&policy->rules[i], &policy->rules[i + 1],
                    (policy->rule_count - i - 1) * sizeof(AccessRule));
            policy->rule_count--;
            return 0;
        }
    }
    return -1;
}

int security_enable_policy(SecurityCore *core, uint32_t policy_id, bool enable)
{
    if (!core)
        return -1;

    SecurityPolicy *policy = security_get_policy(core, policy_id);
    if (!policy)
        return -1;

    policy->is_enabled = enable;
    return 0;
}

SecurityPolicy *security_get_policy(SecurityCore *core, uint32_t policy_id)
{
    if (!core)
        return NULL;

    for (uint32_t i = 0; i < core->policy_count; i++)
    {
        if (core->policies[i].policy_id == policy_id)
        {
            return &core->policies[i];
        }
    }
    return NULL;
}

int security_check_access(SecurityCore *core, uint32_t user_id, uint32_t object_id,
                          Permission required_permission)
{
    if (!core)
        return -1;

    User *user = security_get_user(core, user_id);
    if (!user || !user->is_active)
    {
        core->total_access_denied++;
        return -1;
    }

    if ((user->permissions & required_permission) == 0)
    {
        core->total_access_denied++;
        return -1;
    }

    core->total_access_granted++;
    return 0;
}

int security_audit_action(SecurityCore *core, uint32_t user_id, uint32_t subject_id,
                          uint32_t object_id, AuditAction action, bool success, const char *details)
{
    if (!core || core->audit_log_count >= MAX_AUDIT_LOGS)
        return -1;

    AuditLog *log = &core->audit_logs[core->audit_log_count];
    log->log_id = core->audit_log_count + 1;
    log->action = action;
    log->user_id = user_id;
    log->subject_id = subject_id;
    log->object_id = object_id;
    log->success = success;
    log->timestamp = time(NULL);

    if (details)
    {
        strncpy(log->details, details, 255);
        log->details[255] = '\0';
    }

    core->audit_log_count++;
    return 0;
}

uint32_t security_create_encryption_key(SecurityCore *core, EncryptionMethod method, uint32_t size)
{
    if (!core || core->key_count >= MAX_ENCRYPTION_KEYS)
        return 0;
    if (size > 256)
        return 0;

    EncryptionKey *key = &core->keys[core->key_count];
    key->key_id = core->key_count + 1;
    key->method = method;
    key->key_size = size;
    memset(key->key_data, 0, size);
    key->creation_time = time(NULL);
    key->expiration_time = key->creation_time + (365 * 24 * 3600);
    key->is_active = true;

    core->key_count++;
    return key->key_id;
}

int security_delete_encryption_key(SecurityCore *core, uint32_t key_id)
{
    if (!core)
        return -1;

    for (uint32_t i = 0; i < core->key_count; i++)
    {
        if (core->keys[i].key_id == key_id)
        {
            memmove(&core->keys[i], &core->keys[i + 1],
                    (core->key_count - i - 1) * sizeof(EncryptionKey));
            core->key_count--;
            return 0;
        }
    }
    return -1;
}

EncryptionKey *security_get_encryption_key(SecurityCore *core, uint32_t key_id)
{
    if (!core)
        return NULL;

    for (uint32_t i = 0; i < core->key_count; i++)
    {
        if (core->keys[i].key_id == key_id && core->keys[i].is_active)
        {
            return &core->keys[i];
        }
    }
    return NULL;
}

int security_enable_selinux(SecurityCore *core, bool enable)
{
    if (!core)
        return -1;
    core->selinux_enabled = enable;
    return 0;
}

int security_set_failed_access_threshold(SecurityCore *core, uint32_t threshold)
{
    if (!core || threshold == 0)
        return -1;
    core->failed_access_threshold = threshold;
    return 0;
}

int security_set_lockout_duration(SecurityCore *core, uint32_t seconds)
{
    if (!core || seconds == 0)
        return -1;
    core->lockout_duration_sec = seconds;
    return 0;
}

uint32_t security_get_audit_log_count(SecurityCore *core)
{
    if (!core)
        return 0;
    return core->audit_log_count;
}

AuditLog *security_get_audit_log(SecurityCore *core, uint32_t log_id)
{
    if (!core)
        return NULL;

    for (uint32_t i = 0; i < core->audit_log_count; i++)
    {
        if (core->audit_logs[i].log_id == log_id)
        {
            return &core->audit_logs[i];
        }
    }
    return NULL;
}

uint64_t security_get_total_access_denied(SecurityCore *core)
{
    if (!core)
        return 0;
    return core->total_access_denied;
}

uint64_t security_get_total_access_granted(SecurityCore *core)
{
    if (!core)
        return 0;
    return core->total_access_granted;
}
