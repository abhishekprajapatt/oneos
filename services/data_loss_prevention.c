#include "data_loss_prevention.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static DataLossPrevention g_managers[MAX_DLP_MANAGERS] = {0};
static uint32_t g_manager_count = 0;
static uint64_t g_incident_id_counter = 1;

int dlp_create_manager(const char *name)
{
    if (!name || g_manager_count >= MAX_DLP_MANAGERS)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[g_manager_count];
    manager->manager_id = g_manager_count + 1;
    strncpy(manager->name, name, 62);

    manager->policy_count = 0;
    manager->rule_count = 0;
    manager->pattern_count = 0;
    manager->resource_count = 0;
    manager->destination_count = 0;
    manager->incident_count = 0;

    manager->total_incidents = 0;
    manager->total_violations = 0;
    manager->total_blocked = 0;
    manager->total_quarantined = 0;
    manager->total_encrypted = 0;

    manager->policy_version = 1;
    manager->last_policy_update = time(NULL);
    manager->auto_remediation_enabled = 0;
    manager->is_active = 1;

    g_manager_count++;
    return manager->manager_id;
}

int dlp_delete_manager(uint32_t dlp_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return -1;
    }

    for (uint32_t i = dlp_id - 1; i < g_manager_count - 1; i++)
    {
        g_managers[i] = g_managers[i + 1];
    }
    g_manager_count--;

    return 0;
}

int dlp_add_policy(uint32_t dlp_id, const char *policy_name, DLPPolicyType policy_type,
                   DataClassification min_class)
{
    if (dlp_id == 0 || dlp_id > g_manager_count || !policy_name)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];
    if (manager->policy_count >= MAX_DLP_POLICIES)
    {
        return -1;
    }

    DLPPolicy *policy = &manager->policies[manager->policy_count];
    policy->policy_id = manager->policy_count + 1;
    strncpy(policy->policy_name, policy_name, 126);
    policy->policy_type = policy_type;
    policy->min_classification = min_class;
    policy->rule_count = 0;
    policy->incident_count = 0;
    policy->is_enabled = 1;
    policy->is_global_policy = (dlp_id == 1) ? 1 : 0;
    policy->user_scope = 0xFFFFFFFF;
    policy->created_at = time(NULL);
    policy->last_modified = time(NULL);
    strcpy(policy->description, "");

    manager->policy_count++;
    manager->last_policy_update = time(NULL);

    return policy->policy_id;
}

int dlp_remove_policy(uint32_t dlp_id, uint32_t policy_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];

    for (uint32_t i = 0; i < manager->policy_count; i++)
    {
        if (manager->policies[i].policy_id == policy_id)
        {
            for (uint32_t j = i; j < manager->policy_count - 1; j++)
            {
                manager->policies[j] = manager->policies[j + 1];
            }
            manager->policy_count--;
            return 0;
        }
    }

    return -1;
}

int dlp_enable_policy(uint32_t dlp_id, uint32_t policy_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];

    for (uint32_t i = 0; i < manager->policy_count; i++)
    {
        if (manager->policies[i].policy_id == policy_id)
        {
            manager->policies[i].is_enabled = 1;
            return 0;
        }
    }

    return -1;
}

int dlp_disable_policy(uint32_t dlp_id, uint32_t policy_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];

    for (uint32_t i = 0; i < manager->policy_count; i++)
    {
        if (manager->policies[i].policy_id == policy_id)
        {
            manager->policies[i].is_enabled = 0;
            return 0;
        }
    }

    return -1;
}

int dlp_add_sensitive_pattern(uint32_t dlp_id, const char *pattern_name,
                              const char *pattern_regex, DataClassification data_class)
{
    if (dlp_id == 0 || dlp_id > g_manager_count || !pattern_name || !pattern_regex)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];
    if (manager->pattern_count >= MAX_SENSITIVE_DATA_PATTERNS)
    {
        return -1;
    }

    SensitiveDataPattern *pattern = &manager->patterns[manager->pattern_count];
    pattern->pattern_id = manager->pattern_count + 1;
    strncpy(pattern->pattern_name, pattern_name, 126);
    strncpy(pattern->pattern_regex, pattern_regex, 510);
    pattern->data_class = data_class;
    pattern->match_count = 0;
    pattern->false_positive_count = 0;
    pattern->is_enabled = 1;
    pattern->confidence_level = 85;

    manager->pattern_count++;

    return pattern->pattern_id;
}

int dlp_add_dlp_rule(uint32_t dlp_id, const char *rule_name, DLPRuleType rule_type,
                     DLPAction action, uint32_t pattern_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count || !rule_name)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];
    if (manager->rule_count >= MAX_DLP_RULES)
    {
        return -1;
    }

    DLPRule *rule = &manager->rules[manager->rule_count];
    rule->rule_id = manager->rule_count + 1;
    strncpy(rule->rule_name, rule_name, 126);
    rule->rule_type = rule_type;
    strcpy(rule->rule_expression, "");
    rule->action_on_match = action;
    rule->pattern_id = pattern_id;
    rule->trigger_count = 0;
    rule->is_enabled = 1;
    rule->requires_escalation = (action == DLP_ACTION_BLOCK) ? 1 : 0;
    rule->severity_score = (action * 20) + 10;

    manager->rule_count++;

    return rule->rule_id;
}

int dlp_remove_dlp_rule(uint32_t dlp_id, uint32_t rule_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];

    for (uint32_t i = 0; i < manager->rule_count; i++)
    {
        if (manager->rules[i].rule_id == rule_id)
        {
            for (uint32_t j = i; j < manager->rule_count - 1; j++)
            {
                manager->rules[j] = manager->rules[j + 1];
            }
            manager->rule_count--;
            return 0;
        }
    }

    return -1;
}

int dlp_add_protected_resource(uint32_t dlp_id, const char *resource_name,
                               const char *resource_path, DataClassification classification)
{
    if (dlp_id == 0 || dlp_id > g_manager_count || !resource_name || !resource_path)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];
    if (manager->resource_count >= MAX_PROTECTED_RESOURCES)
    {
        return -1;
    }

    ProtectedResource *resource = &manager->resources[manager->resource_count];
    resource->resource_id = manager->resource_count + 1;
    strncpy(resource->resource_name, resource_name, 126);
    strncpy(resource->resource_path, resource_path, 254);
    resource->classification = classification;
    resource->file_hash = (uint64_t)resource->resource_id * 12345;
    resource->policy_id = 0;
    resource->owner_id = 0;
    resource->access_count = 0;
    resource->created_at = time(NULL);
    resource->last_accessed = time(NULL);
    resource->last_modified = time(NULL);
    resource->is_protected = 1;

    manager->resource_count++;

    return resource->resource_id;
}

int dlp_classify_resource(uint32_t dlp_id, uint32_t resource_id, DataClassification classification)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];

    for (uint32_t i = 0; i < manager->resource_count; i++)
    {
        if (manager->resources[i].resource_id == resource_id)
        {
            manager->resources[i].classification = classification;
            manager->resources[i].last_modified = time(NULL);
            return 0;
        }
    }

    return -1;
}

int dlp_add_allowed_destination(uint32_t dlp_id, const char *dest_name,
                                const char *dest_address, DataClassification min_class)
{
    if (dlp_id == 0 || dlp_id > g_manager_count || !dest_name || !dest_address)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];
    if (manager->destination_count >= MAX_ALLOWED_DESTINATIONS)
    {
        return -1;
    }

    AllowedDestination *dest = &manager->destinations[manager->destination_count];
    dest->destination_id = manager->destination_count + 1;
    strncpy(dest->destination_name, dest_name, 126);
    strncpy(dest->destination_address, dest_address, 254);
    dest->min_data_class = min_class;
    dest->is_trusted = 1;
    dest->requires_encryption = (min_class >= DATA_CLASSIFICATION_CONFIDENTIAL) ? 1 : 0;
    dest->requires_approval = (min_class >= DATA_CLASSIFICATION_SECRET) ? 1 : 0;
    dest->allowed_transfer_rate_kbps = 10240;
    dest->last_access = 0;

    manager->destination_count++;

    return dest->destination_id;
}

int dlp_remove_allowed_destination(uint32_t dlp_id, uint32_t destination_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];

    for (uint32_t i = 0; i < manager->destination_count; i++)
    {
        if (manager->destinations[i].destination_id == destination_id)
        {
            for (uint32_t j = i; j < manager->destination_count - 1; j++)
            {
                manager->destinations[j] = manager->destinations[j + 1];
            }
            manager->destination_count--;
            return 0;
        }
    }

    return -1;
}

int dlp_scan_for_violations(uint32_t dlp_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return -1;
    }

    return 0;
}

int dlp_scan_resource(uint32_t dlp_id, uint32_t resource_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return -1;
    }

    return 0;
}

int dlp_detect_violation(uint32_t dlp_id, uint32_t rule_id, uint32_t source_user_id,
                         const char *source_location, DataClassification data_class,
                         uint32_t record_count, uint64_t data_volume)
{
    if (dlp_id == 0 || dlp_id > g_manager_count || !source_location)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];
    if (manager->incident_count >= MAX_DLP_INCIDENTS)
    {
        return -1;
    }

    DLPIncident *incident = &manager->incidents[manager->incident_count];
    incident->incident_id = g_incident_id_counter++;
    snprintf(incident->incident_name, 126, "DLP_INCIDENT_%lu", incident->incident_id);
    incident->rule_id = rule_id;
    incident->policy_id = 0;
    incident->status = DLP_INCIDENT_STATUS_DETECTED;
    incident->source_user_id = source_user_id;
    strncpy(incident->source_location, source_location, 254);
    incident->data_class = data_class;
    incident->affected_record_count = record_count;
    incident->data_volume_bytes = data_volume;
    incident->action_taken = DLP_ACTION_AUDIT;
    incident->detected_at = time(NULL);
    incident->resolved_at = 0;
    strcpy(incident->remediation_notes, "");
    incident->investigation_duration_minutes = 0;

    for (uint32_t i = 0; i < manager->rule_count; i++)
    {
        if (manager->rules[i].rule_id == rule_id)
        {
            incident->action_taken = manager->rules[i].action_on_match;
            manager->rules[i].trigger_count++;
        }
    }

    manager->incident_count++;
    manager->total_incidents++;
    manager->total_violations++;

    if (incident->action_taken == DLP_ACTION_BLOCK)
    {
        manager->total_blocked++;
    }
    else if (incident->action_taken == DLP_ACTION_QUARANTINE)
    {
        manager->total_quarantined++;
    }

    return incident->incident_id;
}

int dlp_block_data_transfer(uint32_t dlp_id, uint64_t incident_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];

    for (uint32_t i = 0; i < manager->incident_count; i++)
    {
        if (manager->incidents[i].incident_id == incident_id)
        {
            manager->incidents[i].action_taken = DLP_ACTION_BLOCK;
            manager->total_blocked++;
            return 0;
        }
    }

    return -1;
}

int dlp_quarantine_data(uint32_t dlp_id, uint64_t incident_id, const char *quarantine_path)
{
    if (dlp_id == 0 || dlp_id > g_manager_count || !quarantine_path)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];

    for (uint32_t i = 0; i < manager->incident_count; i++)
    {
        if (manager->incidents[i].incident_id == incident_id)
        {
            manager->incidents[i].action_taken = DLP_ACTION_QUARANTINE;
            manager->total_quarantined++;
            return 0;
        }
    }

    return -1;
}

int dlp_encrypt_data(uint32_t dlp_id, uint64_t incident_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];

    for (uint32_t i = 0; i < manager->incident_count; i++)
    {
        if (manager->incidents[i].incident_id == incident_id)
        {
            manager->incidents[i].action_taken = DLP_ACTION_ENCRYPT;
            manager->total_encrypted += manager->incidents[i].data_volume_bytes;
            return 0;
        }
    }

    return -1;
}

int dlp_redact_sensitive_content(uint32_t dlp_id, uint32_t resource_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return -1;
    }

    return 0;
}

int dlp_investigate_incident(uint32_t dlp_id, uint64_t incident_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];

    for (uint32_t i = 0; i < manager->incident_count; i++)
    {
        if (manager->incidents[i].incident_id == incident_id)
        {
            manager->incidents[i].status = DLP_INCIDENT_STATUS_INVESTIGATED;
            return 0;
        }
    }

    return -1;
}

int dlp_resolve_incident(uint32_t dlp_id, uint64_t incident_id, DLPIncidentStatus status)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];

    for (uint32_t i = 0; i < manager->incident_count; i++)
    {
        if (manager->incidents[i].incident_id == incident_id)
        {
            manager->incidents[i].status = status;
            manager->incidents[i].resolved_at = time(NULL);
            return 0;
        }
    }

    return -1;
}

int dlp_get_active_incidents(uint32_t dlp_id, DLPIncident *incidents, uint32_t *count)
{
    if (dlp_id == 0 || dlp_id > g_manager_count || !incidents || !count)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];
    uint32_t active_count = 0;

    for (uint32_t i = 0; i < manager->incident_count && active_count < *count; i++)
    {
        if (manager->incidents[i].status != DLP_INCIDENT_STATUS_RESOLVED)
        {
            incidents[active_count++] = manager->incidents[i];
        }
    }

    *count = active_count;
    return 0;
}

int dlp_get_incident_details(uint32_t dlp_id, uint64_t incident_id, DLPIncident *incident)
{
    if (dlp_id == 0 || dlp_id > g_manager_count || !incident)
    {
        return -1;
    }

    DataLossPrevention *manager = &g_managers[dlp_id - 1];

    for (uint32_t i = 0; i < manager->incident_count; i++)
    {
        if (manager->incidents[i].incident_id == incident_id)
        {
            memcpy(incident, &manager->incidents[i], sizeof(DLPIncident));
            return 0;
        }
    }

    return -1;
}

uint64_t dlp_get_total_violations(uint32_t dlp_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return 0;
    }

    return g_managers[dlp_id - 1].total_violations;
}

uint64_t dlp_get_total_blocked_transfers(uint32_t dlp_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return 0;
    }

    return g_managers[dlp_id - 1].total_blocked;
}

uint64_t dlp_get_total_data_encrypted(uint32_t dlp_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return 0;
    }

    return g_managers[dlp_id - 1].total_encrypted;
}

int dlp_generate_dlp_report(uint32_t dlp_id, const char *output_path)
{
    if (dlp_id == 0 || dlp_id > g_manager_count || !output_path)
    {
        return -1;
    }

    return 0;
}

int dlp_export_dlp_config(uint32_t dlp_id, const char *export_path)
{
    if (dlp_id == 0 || dlp_id > g_manager_count || !export_path)
    {
        return -1;
    }

    return 0;
}

int dlp_enable_auto_remediation(uint32_t dlp_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return -1;
    }

    g_managers[dlp_id - 1].auto_remediation_enabled = 1;
    return 0;
}

int dlp_disable_auto_remediation(uint32_t dlp_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return -1;
    }

    g_managers[dlp_id - 1].auto_remediation_enabled = 0;
    return 0;
}

DataLossPrevention *dlp_get_manager_info(uint32_t dlp_id)
{
    if (dlp_id == 0 || dlp_id > g_manager_count)
    {
        return NULL;
    }

    return &g_managers[dlp_id - 1];
}
