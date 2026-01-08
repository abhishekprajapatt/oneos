#include "self_healing_manager.h"
#include <stdlib.h>
#include <string.h>

static SelfHealingManager g_managers[MAX_HEALING_MANAGERS] = {0};
static uint32_t g_manager_count = 0;

int shm_create_manager(const char *name)
{
    if (g_manager_count >= MAX_HEALING_MANAGERS)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[g_manager_count];
    mgr->manager_id = g_manager_count++;
    strncpy(mgr->name, name, 63);
    mgr->name[63] = '\0';

    mgr->health_check_count = 0;
    mgr->recovery_action_count = 0;
    mgr->fault_incident_count = 0;
    mgr->fault_pattern_count = 0;
    mgr->service_dependency_count = 0;

    mgr->services_monitored = 0;
    mgr->incidents_detected = 0;
    mgr->incidents_auto_resolved = 0;
    mgr->recovery_actions_executed = 0;
    mgr->system_health_score = 100.0;

    mgr->created_at = time(NULL);
    mgr->last_health_assessment = 0;
    mgr->is_active = 1;

    return mgr->manager_id;
}

int shm_delete_manager(uint32_t manager_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    mgr->is_active = 0;
    mgr->health_check_count = 0;
    mgr->recovery_action_count = 0;
    mgr->fault_incident_count = 0;

    return 0;
}

int shm_create_health_check(uint32_t manager_id, const char *check_name,
                            const char *target_service, HealthCheckType check_type,
                            uint32_t interval_seconds)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    if (mgr->health_check_count >= MAX_HEALTH_CHECKS)
    {
        return -1;
    }

    HealthCheck *hc = &mgr->health_checks[mgr->health_check_count];
    hc->check_id = mgr->health_check_count;
    strncpy(hc->check_name, check_name, 63);
    hc->check_name[63] = '\0';
    strncpy(hc->target_service, target_service, 63);
    hc->target_service[63] = '\0';
    hc->check_type = check_type;
    hc->interval_seconds = interval_seconds;
    hc->timeout_seconds = 30;
    hc->failure_threshold = 3;
    hc->consecutive_failures = 0;
    hc->last_check_time = 0;
    hc->last_check_status = HEALTH_STATUS_UNKNOWN;
    hc->is_enabled = 1;

    mgr->health_check_count++;
    mgr->services_monitored++;

    return hc->check_id;
}

int shm_enable_health_check(uint32_t manager_id, uint32_t check_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    if (check_id >= mgr->health_check_count)
    {
        return -1;
    }

    mgr->health_checks[check_id].is_enabled = 1;

    return 0;
}

int shm_disable_health_check(uint32_t manager_id, uint32_t check_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    if (check_id >= mgr->health_check_count)
    {
        return -1;
    }

    mgr->health_checks[check_id].is_enabled = 0;

    return 0;
}

int shm_delete_health_check(uint32_t manager_id, uint32_t check_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    if (check_id >= mgr->health_check_count)
    {
        return -1;
    }

    mgr->health_checks[check_id].is_enabled = 0;

    return 0;
}

int shm_create_recovery_action(uint32_t manager_id, const char *action_name,
                               RecoveryAction action_type, const char *target_service,
                               uint32_t priority)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    if (mgr->recovery_action_count >= MAX_RECOVERY_ACTIONS)
    {
        return -1;
    }

    RecoveryActionPolicy *action = &mgr->recovery_actions[mgr->recovery_action_count];
    action->action_id = mgr->recovery_action_count;
    strncpy(action->action_name, action_name, 63);
    action->action_name[63] = '\0';
    strncpy(action->target_service, target_service, 63);
    action->target_service[63] = '\0';
    action->action_type = action_type;
    action->action_parameters[0] = '\0';
    action->priority = priority;
    action->timeout_seconds = 60;
    action->last_execution_time = 0;
    action->execution_count = 0;
    action->success_count = 0;
    action->is_enabled = 1;

    mgr->recovery_action_count++;

    return action->action_id;
}

int shm_enable_recovery_action(uint32_t manager_id, uint32_t action_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    if (action_id >= mgr->recovery_action_count)
    {
        return -1;
    }

    mgr->recovery_actions[action_id].is_enabled = 1;

    return 0;
}

int shm_disable_recovery_action(uint32_t manager_id, uint32_t action_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    if (action_id >= mgr->recovery_action_count)
    {
        return -1;
    }

    mgr->recovery_actions[action_id].is_enabled = 0;

    return 0;
}

int shm_delete_recovery_action(uint32_t manager_id, uint32_t action_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    if (action_id >= mgr->recovery_action_count)
    {
        return -1;
    }

    mgr->recovery_actions[action_id].is_enabled = 0;

    return 0;
}

int shm_register_service_dependency(uint32_t manager_id, const char *dependent,
                                    const char *dependency, uint8_t is_critical)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    if (mgr->service_dependency_count >= MAX_SERVICE_DEPENDENCIES)
    {
        return -1;
    }

    ServiceDependency *dep = &mgr->service_dependencies[mgr->service_dependency_count];
    dep->dependency_id = mgr->service_dependency_count;
    strncpy(dep->dependent_service, dependent, 63);
    dep->dependent_service[63] = '\0';
    strncpy(dep->dependency_service, dependency, 63);
    dep->dependency_service[63] = '\0';
    dep->is_critical = is_critical;
    dep->timeout_seconds = 30;

    mgr->service_dependency_count++;

    return dep->dependency_id;
}

int shm_unregister_service_dependency(uint32_t manager_id, uint32_t dependency_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    if (dependency_id >= mgr->service_dependency_count)
    {
        return -1;
    }

    if (dependency_id + 1 == mgr->service_dependency_count)
    {
        mgr->service_dependency_count--;
    }

    return 0;
}

int shm_detect_fault(uint32_t manager_id, FaultType fault_type, SeverityLevel severity,
                     const char *description, const char *affected_component)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    if (mgr->fault_incident_count >= MAX_FAULT_PATTERNS)
    {
        return -1;
    }

    FaultIncident *incident = &mgr->fault_incidents[mgr->fault_incident_count];
    incident->incident_id = mgr->fault_incident_count;
    incident->fault_type = fault_type;
    incident->severity = severity;
    strncpy(incident->description, description, 255);
    incident->description[255] = '\0';
    strncpy(incident->affected_component, affected_component, 63);
    incident->affected_component[63] = '\0';
    incident->detection_time = time(NULL);
    incident->resolution_time = 0;
    incident->impact_score = (uint32_t)severity * 25;
    incident->actions_taken = 0;
    incident->auto_resolved = 0;

    mgr->fault_incident_count++;
    mgr->incidents_detected++;

    return incident->incident_id;
}

int shm_execute_recovery_action(uint32_t manager_id, uint32_t incident_id,
                                RecoveryAction action)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    if (incident_id >= mgr->fault_incident_count)
    {
        return -1;
    }

    FaultIncident *incident = &mgr->fault_incidents[incident_id];
    incident->actions_taken = action;

    mgr->recovery_actions_executed++;

    return 0;
}

int shm_resolve_incident(uint32_t manager_id, uint32_t incident_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    if (incident_id >= mgr->fault_incident_count)
    {
        return -1;
    }

    FaultIncident *incident = &mgr->fault_incidents[incident_id];
    incident->resolution_time = time(NULL);
    incident->auto_resolved = 1;

    mgr->incidents_auto_resolved++;

    return 0;
}

int shm_register_fault_pattern(uint32_t manager_id, const char *pattern_name,
                               FaultType fault_type, const char *symptom_description,
                               RecoveryAction recommended_action)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    if (mgr->fault_pattern_count >= MAX_FAULT_PATTERNS)
    {
        return -1;
    }

    FaultPattern *pattern = &mgr->fault_patterns[mgr->fault_pattern_count];
    pattern->pattern_id = mgr->fault_pattern_count;
    strncpy(pattern->pattern_name, pattern_name, 63);
    pattern->pattern_name[63] = '\0';
    pattern->fault_type = fault_type;
    strncpy(pattern->symptom_description, symptom_description, 255);
    pattern->symptom_description[255] = '\0';
    pattern->detection_confidence = 80;
    pattern->recommended_action = recommended_action;
    pattern->occurrences = 0;
    pattern->last_detected = 0;

    mgr->fault_pattern_count++;

    return pattern->pattern_id;
}

int shm_match_fault_pattern(uint32_t manager_id, FaultType fault_type,
                            const char *symptoms, uint32_t *pattern_id)
{
    if (manager_id >= g_manager_count || !pattern_id)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];

    for (uint32_t i = 0; i < mgr->fault_pattern_count; i++)
    {
        FaultPattern *pattern = &mgr->fault_patterns[i];
        if (pattern->fault_type == fault_type &&
            strstr(pattern->symptom_description, symptoms) != NULL)
        {
            *pattern_id = i;
            pattern->occurrences++;
            pattern->last_detected = time(NULL);
            return 0;
        }
    }

    return -1;
}

int shm_perform_health_assessment(uint32_t manager_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    mgr->last_health_assessment = time(NULL);

    double total_health = 0.0;
    uint32_t healthy_count = 0;

    for (uint32_t i = 0; i < mgr->health_check_count; i++)
    {
        if (mgr->health_checks[i].is_enabled)
        {
            switch (mgr->health_checks[i].last_check_status)
            {
            case HEALTH_STATUS_HEALTHY:
                total_health += 100.0;
                healthy_count++;
                break;
            case HEALTH_STATUS_DEGRADED:
                total_health += 70.0;
                healthy_count++;
                break;
            case HEALTH_STATUS_UNHEALTHY:
                total_health += 30.0;
                healthy_count++;
                break;
            case HEALTH_STATUS_RECOVERING:
                total_health += 50.0;
                healthy_count++;
                break;
            default:
                total_health += 0.0;
                break;
            }
        }
    }

    if (healthy_count > 0)
    {
        mgr->system_health_score = total_health / (double)healthy_count;
    }

    return 0;
}

double shm_get_system_health_score(uint32_t manager_id)
{
    if (manager_id >= g_manager_count)
    {
        return 0.0;
    }

    return g_managers[manager_id].system_health_score;
}

HealthStatus shm_get_service_health(uint32_t manager_id, const char *service_name)
{
    if (manager_id >= g_manager_count || !service_name)
    {
        return HEALTH_STATUS_UNKNOWN;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];

    for (uint32_t i = 0; i < mgr->health_check_count; i++)
    {
        if (strcmp(mgr->health_checks[i].target_service, service_name) == 0)
        {
            return mgr->health_checks[i].last_check_status;
        }
    }

    return HEALTH_STATUS_UNKNOWN;
}

int shm_get_incidents(uint32_t manager_id, FaultIncident *incidents, uint32_t *count)
{
    if (manager_id >= g_manager_count || !incidents || !count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    uint32_t idx = 0;

    for (uint32_t i = 0; i < mgr->fault_incident_count && idx < *count; i++)
    {
        if (!mgr->fault_incidents[i].auto_resolved)
        {
            incidents[idx++] = mgr->fault_incidents[i];
        }
    }

    *count = idx;

    return 0;
}

int shm_get_recovery_actions_for_service(uint32_t manager_id, const char *service_name,
                                         RecoveryActionPolicy *actions, uint32_t *count)
{
    if (manager_id >= g_manager_count || !service_name || !actions || !count)
    {
        return -1;
    }

    SelfHealingManager *mgr = &g_managers[manager_id];
    uint32_t idx = 0;

    for (uint32_t i = 0; i < mgr->recovery_action_count && idx < *count; i++)
    {
        if (strcmp(mgr->recovery_actions[i].target_service, service_name) == 0 &&
            mgr->recovery_actions[i].is_enabled)
        {
            actions[idx++] = mgr->recovery_actions[i];
        }
    }

    *count = idx;

    return 0;
}
