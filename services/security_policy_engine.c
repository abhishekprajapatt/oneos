#include "security_policy_engine.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static SecurityPolicyEngine g_engines[MAX_POLICY_ENGINES] = {0};
static uint32_t g_engine_count = 0;
static uint64_t g_violation_id_counter = 1;

int spe_create_engine(const char *name)
{
    if (!name || g_engine_count >= MAX_POLICY_ENGINES)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[g_engine_count];
    engine->engine_id = g_engine_count + 1;
    strncpy(engine->name, name, 62);

    engine->policy_count = 0;
    engine->role_count = 0;
    engine->group_count = 0;
    engine->violation_count = 0;

    engine->total_violations = 0;
    engine->violations_remediated = 0;
    engine->violations_escalated = 0;

    engine->enforcement_level = 2;
    engine->is_active = 1;
    engine->last_policy_sync = time(NULL);

    g_engine_count++;
    return engine->engine_id;
}

int spe_delete_engine(uint32_t engine_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    for (uint32_t i = engine_id - 1; i < g_engine_count - 1; i++)
    {
        g_engines[i] = g_engines[i + 1];
    }
    g_engine_count--;

    return 0;
}

int spe_add_security_policy(uint32_t engine_id, const char *policy_name,
                            PolicyType policy_type, const char *description)
{
    if (engine_id == 0 || engine_id > g_engine_count || !policy_name)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];
    if (engine->policy_count >= MAX_SECURITY_POLICIES)
    {
        return -1;
    }

    SecurityPolicy *policy = &engine->policies[engine->policy_count];
    policy->policy_id = engine->policy_count + 1;
    strncpy(policy->policy_name, policy_name, 126);
    policy->policy_type = policy_type;
    policy->state = POLICY_STATE_DRAFT;
    if (description)
    {
        strncpy(policy->description, description, 254);
    }
    policy->rule_count = 0;
    policy->affected_users = 0;
    policy->affected_resources = 0;
    policy->violation_count = 0;
    policy->is_mandatory = 0;
    policy->created_at = time(NULL);
    policy->modified_at = time(NULL);
    policy->enforced_at = 0;
    policy->policy_version = 1;

    engine->policy_count++;

    return policy->policy_id;
}

int spe_remove_security_policy(uint32_t engine_id, uint32_t policy_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->policy_count; i++)
    {
        if (engine->policies[i].policy_id == policy_id)
        {
            for (uint32_t j = i; j < engine->policy_count - 1; j++)
            {
                engine->policies[j] = engine->policies[j + 1];
            }
            engine->policy_count--;
            return 0;
        }
    }

    return -1;
}

int spe_activate_policy(uint32_t engine_id, uint32_t policy_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->policy_count; i++)
    {
        if (engine->policies[i].policy_id == policy_id)
        {
            engine->policies[i].state = POLICY_STATE_ACTIVE;
            engine->policies[i].modified_at = time(NULL);
            return 0;
        }
    }

    return -1;
}

int spe_deactivate_policy(uint32_t engine_id, uint32_t policy_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->policy_count; i++)
    {
        if (engine->policies[i].policy_id == policy_id)
        {
            engine->policies[i].state = POLICY_STATE_DEPRECATED;
            engine->policies[i].modified_at = time(NULL);
            return 0;
        }
    }

    return -1;
}

int spe_enforce_policy(uint32_t engine_id, uint32_t policy_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->policy_count; i++)
    {
        if (engine->policies[i].policy_id == policy_id)
        {
            engine->policies[i].state = POLICY_STATE_ENFORCED;
            engine->policies[i].enforced_at = time(NULL);
            engine->policies[i].is_mandatory = 1;
            return 0;
        }
    }

    return -1;
}

int spe_create_user_role(uint32_t engine_id, const char *role_name)
{
    if (engine_id == 0 || engine_id > g_engine_count || !role_name)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];
    if (engine->role_count >= MAX_USER_ROLES)
    {
        return -1;
    }

    UserRoleDefinition *role = &engine->roles[engine->role_count];
    role->role_id = engine->role_count + 1;
    strncpy(role->role_name, role_name, 62);
    role->permission_count = 0;
    role->permissions_bitmask = 0;
    role->user_count = 0;
    role->is_system_role = 0;
    role->created_at = time(NULL);

    engine->role_count++;

    return role->role_id;
}

int spe_delete_user_role(uint32_t engine_id, uint32_t role_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->role_count; i++)
    {
        if (engine->roles[i].role_id == role_id)
        {
            for (uint32_t j = i; j < engine->role_count - 1; j++)
            {
                engine->roles[j] = engine->roles[j + 1];
            }
            engine->role_count--;
            return 0;
        }
    }

    return -1;
}

int spe_grant_permission(uint32_t engine_id, uint32_t role_id,
                         const char *permission_name, uint32_t permission_flags)
{
    if (engine_id == 0 || engine_id > g_engine_count || !permission_name)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->role_count; i++)
    {
        if (engine->roles[i].role_id == role_id)
        {
            if (engine->roles[i].permission_count >= MAX_PERMISSIONS)
            {
                return -1;
            }

            ResourcePermission *perm = &engine->roles[i].permissions[engine->roles[i].permission_count];
            perm->permission_id = engine->roles[i].permission_count + 1;
            strncpy(perm->permission_name, permission_name, 62);
            perm->permission_flags = permission_flags;
            strcpy(perm->resource_type, "");
            perm->is_inherited = 0;

            engine->roles[i].permission_count++;
            engine->roles[i].permissions_bitmask |= permission_flags;

            return perm->permission_id;
        }
    }

    return -1;
}

int spe_revoke_permission(uint32_t engine_id, uint32_t role_id, uint32_t permission_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->role_count; i++)
    {
        if (engine->roles[i].role_id == role_id)
        {
            for (uint32_t j = 0; j < engine->roles[i].permission_count; j++)
            {
                if (engine->roles[i].permissions[j].permission_id == permission_id)
                {
                    for (uint32_t k = j; k < engine->roles[i].permission_count - 1; k++)
                    {
                        engine->roles[i].permissions[k] = engine->roles[i].permissions[k + 1];
                    }
                    engine->roles[i].permission_count--;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int spe_add_policy_rule(uint32_t engine_id, uint32_t policy_id,
                        const char *rule_name, const char *condition)
{
    if (engine_id == 0 || engine_id > g_engine_count || !rule_name || !condition)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];
    if (engine->rule_count >= MAX_POLICY_RULES)
    {
        return -1;
    }

    PolicyRule *rule = &engine->rules[engine->rule_count];
    rule->rule_id = engine->rule_count + 1;
    rule->policy_id = policy_id;
    strncpy(rule->rule_name, rule_name, 126);
    strncpy(rule->rule_condition, condition, 510);
    rule->resource_id = 0;
    rule->user_role_id = 0;
    rule->action_mask = 0;
    rule->is_enabled = 1;
    rule->rule_priority = 100;
    rule->violation_severity = VIOLATION_SEVERITY_MEDIUM;

    engine->rule_count++;

    return rule->rule_id;
}

int spe_remove_policy_rule(uint32_t engine_id, uint32_t rule_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->rule_count; i++)
    {
        if (engine->rules[i].rule_id == rule_id)
        {
            for (uint32_t j = i; j < engine->rule_count - 1; j++)
            {
                engine->rules[j] = engine->rules[j + 1];
            }
            engine->rule_count--;
            return 0;
        }
    }

    return -1;
}

int spe_create_security_group(uint32_t engine_id, const char *group_name, uint32_t role_id)
{
    if (engine_id == 0 || engine_id > g_engine_count || !group_name)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];
    if (engine->group_count >= MAX_SECURITY_GROUPS)
    {
        return -1;
    }

    SecurityGroup *group = &engine->groups[engine->group_count];
    group->group_id = engine->group_count + 1;
    strncpy(group->group_name, group_name, 62);
    strcpy(group->group_description, "");
    group->member_count = 0;
    group->role_id = role_id;
    group->created_at = time(NULL);
    group->is_dynamic = 0;

    engine->group_count++;

    return group->group_id;
}

int spe_delete_security_group(uint32_t engine_id, uint32_t group_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->group_count; i++)
    {
        if (engine->groups[i].group_id == group_id)
        {
            for (uint32_t j = i; j < engine->group_count - 1; j++)
            {
                engine->groups[j] = engine->groups[j + 1];
            }
            engine->group_count--;
            return 0;
        }
    }

    return -1;
}

int spe_check_policy_compliance(uint32_t engine_id, uint32_t user_id, uint32_t resource_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    return 1;
}

int spe_detect_policy_violation(uint32_t engine_id, uint32_t policy_id, uint32_t rule_id,
                                uint32_t user_id, uint32_t resource_id,
                                const char *violation_description)
{
    if (engine_id == 0 || engine_id > g_engine_count || !violation_description)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];
    if (engine->violation_count >= MAX_POLICY_VIOLATIONS)
    {
        return -1;
    }

    PolicyViolation *violation = &engine->violations[engine->violation_count];
    violation->violation_id = g_violation_id_counter++;
    violation->policy_id = policy_id;
    violation->rule_id = rule_id;
    violation->user_id = user_id;
    violation->resource_id = resource_id;
    violation->severity = VIOLATION_SEVERITY_MEDIUM;
    strncpy(violation->violation_description, violation_description, 254);
    violation->detected_at = time(NULL);
    violation->resolved_at = 0;
    violation->is_remediated = 0;
    violation->requires_audit = 1;

    engine->violation_count++;
    engine->total_violations++;

    return violation->violation_id;
}

int spe_remediate_violation(uint32_t engine_id, uint64_t violation_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->violation_count; i++)
    {
        if (engine->violations[i].violation_id == violation_id)
        {
            engine->violations[i].is_remediated = 1;
            engine->violations[i].resolved_at = time(NULL);
            engine->violations_remediated++;
            return 0;
        }
    }

    return -1;
}

int spe_escalate_violation(uint32_t engine_id, uint64_t violation_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->violation_count; i++)
    {
        if (engine->violations[i].violation_id == violation_id)
        {
            engine->violations_escalated++;
            return 0;
        }
    }

    return -1;
}

int spe_get_policy_violations(uint32_t engine_id, PolicyViolation *violations, uint32_t *count)
{
    if (engine_id == 0 || engine_id > g_engine_count || !violations || !count)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];

    if (engine->violation_count > *count)
    {
        return -1;
    }

    memcpy(violations, engine->violations, engine->violation_count * sizeof(PolicyViolation));
    *count = engine->violation_count;

    return 0;
}

int spe_get_user_permissions(uint32_t engine_id, uint32_t user_id, uint32_t *permissions)
{
    if (engine_id == 0 || engine_id > g_engine_count || !permissions)
    {
        return -1;
    }

    *permissions = PERMISSION_READ | PERMISSION_WRITE;
    return 0;
}

int spe_audit_policy_changes(uint32_t engine_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    return 0;
}

int spe_sync_policies(uint32_t engine_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    SecurityPolicyEngine *engine = &g_engines[engine_id - 1];
    engine->last_policy_sync = time(NULL);

    return 0;
}

uint64_t spe_get_total_violations(uint32_t engine_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return 0;
    }

    return g_engines[engine_id - 1].total_violations;
}

uint64_t spe_get_violations_remediated(uint32_t engine_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return 0;
    }

    return g_engines[engine_id - 1].violations_remediated;
}

int spe_generate_policy_report(uint32_t engine_id, const char *output_path)
{
    if (engine_id == 0 || engine_id > g_engine_count || !output_path)
    {
        return -1;
    }

    return 0;
}

int spe_export_policy_config(uint32_t engine_id, const char *export_path)
{
    if (engine_id == 0 || engine_id > g_engine_count || !export_path)
    {
        return -1;
    }

    return 0;
}

SecurityPolicyEngine *spe_get_engine_info(uint32_t engine_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return NULL;
    }

    return &g_engines[engine_id - 1];
}
