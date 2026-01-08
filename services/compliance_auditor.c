#include "compliance_auditor.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static ComplianceAuditor g_auditors[MAX_COMPLIANCE_AUDITORS] = {0};
static uint32_t g_auditor_count = 0;
static uint64_t g_violation_id_counter = 1;
static uint64_t g_audit_id_counter = 1;

int ca_create_auditor(const char *name)
{
    if (!name || g_auditor_count >= MAX_COMPLIANCE_AUDITORS)
    {
        return -1;
    }

    ComplianceAuditor *auditor = &g_auditors[g_auditor_count];
    auditor->auditor_id = g_auditor_count + 1;
    strncpy(auditor->name, name, 63);

    auditor->framework_count = 0;
    auditor->control_count = 0;
    auditor->violation_count = 0;
    auditor->audit_trail_count = 0;
    auditor->remediation_count = 0;

    auditor->total_violations_detected = 0;
    auditor->total_violations_resolved = 0;
    auditor->total_audit_events = 0;
    auditor->overall_compliance_score = 100.0;

    auditor->continuous_monitoring_enabled = 0;
    auditor->created_at = time(NULL);
    auditor->last_scan = 0;
    auditor->is_active = 1;

    g_auditor_count++;
    return auditor->auditor_id;
}

int ca_delete_auditor(uint32_t auditor_id)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count)
    {
        return -1;
    }

    for (uint32_t i = auditor_id - 1; i < g_auditor_count - 1; i++)
    {
        g_auditors[i] = g_auditors[i + 1];
    }
    g_auditor_count--;

    return 0;
}

int ca_add_framework(uint32_t auditor_id, ComplianceFramework framework)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count)
    {
        return -1;
    }

    ComplianceAuditor *auditor = &g_auditors[auditor_id - 1];
    if (auditor->framework_count >= MAX_COMPLIANCE_FRAMEWORKS)
    {
        return -1;
    }

    auditor->frameworks[auditor->framework_count] = framework;

    auditor->framework_info[auditor->framework_count].framework_id = auditor->framework_count + 1;
    auditor->framework_info[auditor->framework_count].framework_type = framework;
    auditor->framework_info[auditor->framework_count].total_controls = 0;
    auditor->framework_info[auditor->framework_count].compliant_controls = 0;
    auditor->framework_info[auditor->framework_count].non_compliant_controls = 0;
    auditor->framework_info[auditor->framework_count].partial_compliant_controls = 0;
    auditor->framework_info[auditor->framework_count].is_certified = 0;
    auditor->framework_info[auditor->framework_count].last_audit = 0;
    auditor->framework_info[auditor->framework_count].compliance_score = 0.0;

    switch (framework)
    {
    case FRAMEWORK_HIPAA:
        strcpy(auditor->framework_info[auditor->framework_count].framework_name, "HIPAA");
        break;
    case FRAMEWORK_PCI_DSS:
        strcpy(auditor->framework_info[auditor->framework_count].framework_name, "PCI-DSS");
        break;
    case FRAMEWORK_SOC2:
        strcpy(auditor->framework_info[auditor->framework_count].framework_name, "SOC2");
        break;
    case FRAMEWORK_ISO_27001:
        strcpy(auditor->framework_info[auditor->framework_count].framework_name, "ISO-27001");
        break;
    case FRAMEWORK_GDPR:
        strcpy(auditor->framework_info[auditor->framework_count].framework_name, "GDPR");
        break;
    case FRAMEWORK_NIST:
        strcpy(auditor->framework_info[auditor->framework_count].framework_name, "NIST");
        break;
    case FRAMEWORK_CIS:
        strcpy(auditor->framework_info[auditor->framework_count].framework_name, "CIS");
        break;
    default:
        strcpy(auditor->framework_info[auditor->framework_count].framework_name, "CUSTOM");
        break;
    }

    auditor->framework_count++;
    return 0;
}

int ca_remove_framework(uint32_t auditor_id, ComplianceFramework framework)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count)
    {
        return -1;
    }

    ComplianceAuditor *auditor = &g_auditors[auditor_id - 1];

    for (uint32_t i = 0; i < auditor->framework_count; i++)
    {
        if (auditor->frameworks[i] == framework)
        {
            for (uint32_t j = i; j < auditor->framework_count - 1; j++)
            {
                auditor->frameworks[j] = auditor->frameworks[j + 1];
            }
            auditor->framework_count--;
            return 0;
        }
    }

    return -1;
}

int ca_add_control(uint32_t auditor_id, const char *control_name, ComplianceFramework framework,
                   const char *control_description, uint8_t is_critical)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count || !control_name || !control_description)
    {
        return -1;
    }

    ComplianceAuditor *auditor = &g_auditors[auditor_id - 1];
    if (auditor->control_count >= MAX_COMPLIANCE_CONTROLS)
    {
        return -1;
    }

    ComplianceControl *control = &auditor->controls[auditor->control_count];
    control->control_id = auditor->control_count + 1;
    strncpy(control->control_name, control_name, 127);
    control->framework = framework;
    strncpy(control->control_description, control_description, 255);
    control->status = CONTROL_STATUS_NOT_ASSESSED;
    strcpy(control->requirements, "");
    control->last_assessed = 0;
    control->is_critical = is_critical;
    control->assessment_count = 0;

    auditor->control_count++;

    for (uint32_t i = 0; i < auditor->framework_count; i++)
    {
        if (auditor->frameworks[i] == framework)
        {
            auditor->framework_info[i].total_controls++;
            break;
        }
    }

    return control->control_id;
}

int ca_assess_control(uint32_t auditor_id, uint32_t control_id, ControlStatus status)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count)
    {
        return -1;
    }

    ComplianceAuditor *auditor = &g_auditors[auditor_id - 1];

    for (uint32_t i = 0; i < auditor->control_count; i++)
    {
        if (auditor->controls[i].control_id == control_id)
        {
            auditor->controls[i].status = status;
            auditor->controls[i].last_assessed = time(NULL);
            auditor->controls[i].assessment_count++;

            for (uint32_t j = 0; j < auditor->framework_count; j++)
            {
                if (auditor->frameworks[j] == auditor->controls[i].framework)
                {
                    if (status == CONTROL_STATUS_COMPLIANT)
                    {
                        auditor->framework_info[j].compliant_controls++;
                    }
                    else if (status == CONTROL_STATUS_NON_COMPLIANT)
                    {
                        auditor->framework_info[j].non_compliant_controls++;
                    }
                    else if (status == CONTROL_STATUS_PARTIAL_COMPLIANT)
                    {
                        auditor->framework_info[j].partial_compliant_controls++;
                    }
                    break;
                }
            }

            return 0;
        }
    }

    return -1;
}

int ca_get_controls_by_framework(uint32_t auditor_id, ComplianceFramework framework,
                                 ComplianceControl *controls, uint32_t *count)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count || !controls || !count)
    {
        return -1;
    }

    ComplianceAuditor *auditor = &g_auditors[auditor_id - 1];
    uint32_t found = 0;

    for (uint32_t i = 0; i < auditor->control_count && found < *count; i++)
    {
        if (auditor->controls[i].framework == framework)
        {
            controls[found] = auditor->controls[i];
            found++;
        }
    }

    *count = found;
    return 0;
}

int ca_detect_violation(uint32_t auditor_id, const char *violation_desc,
                        ComplianceFramework framework, uint32_t control_id,
                        ViolationSeverity severity)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count || !violation_desc)
    {
        return -1;
    }

    ComplianceAuditor *auditor = &g_auditors[auditor_id - 1];
    if (auditor->violation_count >= MAX_COMPLIANCE_VIOLATIONS)
    {
        return -1;
    }

    ComplianceViolation *violation = &auditor->violations[auditor->violation_count];
    violation->violation_id = g_violation_id_counter++;
    strncpy(violation->violation_description, violation_desc, 255);
    violation->framework = framework;
    violation->control_id = control_id;
    violation->severity = severity;
    violation->detected_at = time(NULL);
    violation->remediated_at = 0;
    violation->days_open = 0;
    violation->is_resolved = 0;
    strcpy(violation->remediation_notes, "");

    auditor->violation_count++;
    auditor->total_violations_detected++;
    auditor->overall_compliance_score -= (severity * 10);

    return violation->violation_id;
}

int ca_resolve_violation(uint32_t auditor_id, uint64_t violation_id,
                         const char *remediation_notes)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count || !remediation_notes)
    {
        return -1;
    }

    ComplianceAuditor *auditor = &g_auditors[auditor_id - 1];

    for (uint32_t i = 0; i < auditor->violation_count; i++)
    {
        if (auditor->violations[i].violation_id == violation_id)
        {
            auditor->violations[i].remediated_at = time(NULL);
            auditor->violations[i].days_open =
                (uint32_t)((auditor->violations[i].remediated_at - auditor->violations[i].detected_at) / 86400);
            auditor->violations[i].is_resolved = 1;
            strncpy(auditor->violations[i].remediation_notes, remediation_notes, 255);

            auditor->total_violations_resolved++;
            auditor->overall_compliance_score += (auditor->violations[i].severity * 5);

            return 0;
        }
    }

    return -1;
}

int ca_get_violations(uint32_t auditor_id, ComplianceViolation *violations, uint32_t *count)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count || !violations || !count)
    {
        return -1;
    }

    ComplianceAuditor *auditor = &g_auditors[auditor_id - 1];

    if (auditor->violation_count > *count)
    {
        return -1;
    }

    memcpy(violations, auditor->violations, auditor->violation_count * sizeof(ComplianceViolation));
    *count = auditor->violation_count;

    return 0;
}

int ca_create_remediation_item(uint32_t auditor_id, uint64_t violation_id,
                               const char *remediation_plan, uint32_t responsible_team_id)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count || !remediation_plan)
    {
        return -1;
    }

    ComplianceAuditor *auditor = &g_auditors[auditor_id - 1];
    if (auditor->remediation_count >= MAX_REMEDIATION_ITEMS)
    {
        return -1;
    }

    RemediationItem *item = &auditor->remediation_items[auditor->remediation_count];
    item->remediation_id = auditor->remediation_count + 1;
    item->violation_id = violation_id;

    for (uint32_t i = 0; i < auditor->violation_count; i++)
    {
        if (auditor->violations[i].violation_id == violation_id)
        {
            item->framework = auditor->violations[i].framework;
            break;
        }
    }

    strncpy(item->remediation_plan, remediation_plan, 511);
    item->status = REMEDIATION_STATUS_PENDING;
    item->created_at = time(NULL);
    item->target_completion = time(NULL) + (30 * 86400);
    item->actual_completion = 0;
    item->responsible_team_id = responsible_team_id;
    strcpy(item->responsible_name, "");

    auditor->remediation_count++;

    return item->remediation_id;
}

int ca_update_remediation_status(uint32_t auditor_id, uint32_t remediation_id,
                                 RemediationStatus status)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count)
    {
        return -1;
    }

    ComplianceAuditor *auditor = &g_auditors[auditor_id - 1];

    for (uint32_t i = 0; i < auditor->remediation_count; i++)
    {
        if (auditor->remediation_items[i].remediation_id == remediation_id)
        {
            auditor->remediation_items[i].status = status;

            if (status == REMEDIATION_STATUS_COMPLETED || status == REMEDIATION_STATUS_VERIFIED)
            {
                auditor->remediation_items[i].actual_completion = time(NULL);
            }

            return 0;
        }
    }

    return -1;
}

int ca_get_remediation_items(uint32_t auditor_id, RemediationItem *items, uint32_t *count)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count || !items || !count)
    {
        return -1;
    }

    ComplianceAuditor *auditor = &g_auditors[auditor_id - 1];

    if (auditor->remediation_count > *count)
    {
        return -1;
    }

    memcpy(items, auditor->remediation_items, auditor->remediation_count * sizeof(RemediationItem));
    *count = auditor->remediation_count;

    return 0;
}

int ca_log_audit_event(uint32_t auditor_id, uint32_t actor_id, AuditAction action,
                       const char *resource_path, const char *ip_address, uint8_t success,
                       const char *details)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count || !resource_path || !ip_address || !details)
    {
        return -1;
    }

    ComplianceAuditor *auditor = &g_auditors[auditor_id - 1];
    if (auditor->audit_trail_count >= MAX_AUDIT_TRAILS)
    {
        return -1;
    }

    AuditTrail *trail = &auditor->audit_trails[auditor->audit_trail_count];
    trail->audit_id = g_audit_id_counter++;
    trail->actor_id = actor_id;
    trail->action = action;
    strncpy(trail->resource_path, resource_path, 255);
    strncpy(trail->ip_address, ip_address, 44);
    trail->action_timestamp = time(NULL);
    trail->was_successful = success;
    strncpy(trail->details, details, 511);

    auditor->audit_trail_count++;
    auditor->total_audit_events++;

    return trail->audit_id;
}

int ca_get_audit_trails(uint32_t auditor_id, AuditTrail *trails, uint32_t *count)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count || !trails || !count)
    {
        return -1;
    }

    ComplianceAuditor *auditor = &g_auditors[auditor_id - 1];

    if (auditor->audit_trail_count > *count)
    {
        return -1;
    }

    memcpy(trails, auditor->audit_trails, auditor->audit_trail_count * sizeof(AuditTrail));
    *count = auditor->audit_trail_count;

    return 0;
}

int ca_get_audit_events_by_action(uint32_t auditor_id, AuditAction action,
                                  AuditTrail *trails, uint32_t *count)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count || !trails || !count)
    {
        return -1;
    }

    ComplianceAuditor *auditor = &g_auditors[auditor_id - 1];
    uint32_t found = 0;

    for (uint32_t i = 0; i < auditor->audit_trail_count && found < *count; i++)
    {
        if (auditor->audit_trails[i].action == action)
        {
            trails[found] = auditor->audit_trails[i];
            found++;
        }
    }

    *count = found;
    return 0;
}

int ca_calculate_compliance_score(uint32_t auditor_id, ComplianceFramework framework)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count)
    {
        return -1;
    }

    ComplianceAuditor *auditor = &g_auditors[auditor_id - 1];

    for (uint32_t i = 0; i < auditor->framework_count; i++)
    {
        if (auditor->frameworks[i] == framework)
        {
            uint32_t total = auditor->framework_info[i].total_controls;
            if (total == 0)
                return -1;

            uint32_t compliant = auditor->framework_info[i].compliant_controls;
            auditor->framework_info[i].compliance_score = ((double)compliant / total) * 100.0;
            auditor->framework_info[i].last_audit = time(NULL);

            if (auditor->framework_info[i].compliance_score >= 95.0)
            {
                auditor->framework_info[i].is_certified = 1;
                auditor->framework_info[i].certification_expiry = time(NULL) + (365 * 86400);
            }

            return 0;
        }
    }

    return -1;
}

int ca_generate_compliance_report(uint32_t auditor_id, ComplianceFramework framework,
                                  const char *output_path)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count || !output_path)
    {
        return -1;
    }

    return 0;
}

int ca_enable_continuous_monitoring(uint32_t auditor_id)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count)
    {
        return -1;
    }

    g_auditors[auditor_id - 1].continuous_monitoring_enabled = 1;
    return 0;
}

int ca_disable_continuous_monitoring(uint32_t auditor_id)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count)
    {
        return -1;
    }

    g_auditors[auditor_id - 1].continuous_monitoring_enabled = 0;
    return 0;
}

int ca_export_audit_log(uint32_t auditor_id, const char *export_path)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count || !export_path)
    {
        return -1;
    }

    return 0;
}

int ca_export_compliance_certification(uint32_t auditor_id, ComplianceFramework framework,
                                       const char *cert_path)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count || !cert_path)
    {
        return -1;
    }

    return 0;
}

uint64_t ca_get_total_violations_detected(uint32_t auditor_id)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count)
    {
        return 0;
    }

    return g_auditors[auditor_id - 1].total_violations_detected;
}

uint64_t ca_get_total_violations_resolved(uint32_t auditor_id)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count)
    {
        return 0;
    }

    return g_auditors[auditor_id - 1].total_violations_resolved;
}

uint64_t ca_get_total_audit_events(uint32_t auditor_id)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count)
    {
        return 0;
    }

    return g_auditors[auditor_id - 1].total_audit_events;
}

double ca_get_overall_compliance_score(uint32_t auditor_id)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count)
    {
        return 0.0;
    }

    return g_auditors[auditor_id - 1].overall_compliance_score;
}

double ca_get_framework_compliance_score(uint32_t auditor_id, ComplianceFramework framework)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count)
    {
        return 0.0;
    }

    ComplianceAuditor *auditor = &g_auditors[auditor_id - 1];

    for (uint32_t i = 0; i < auditor->framework_count; i++)
    {
        if (auditor->frameworks[i] == framework)
        {
            return auditor->framework_info[i].compliance_score;
        }
    }

    return 0.0;
}

ComplianceAuditor *ca_get_auditor_info(uint32_t auditor_id)
{
    if (auditor_id == 0 || auditor_id > g_auditor_count)
    {
        return NULL;
    }

    return &g_auditors[auditor_id - 1];
}
