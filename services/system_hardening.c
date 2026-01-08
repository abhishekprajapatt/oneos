#include "system_hardening.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static SystemHardening g_managers[MAX_HARDENING_MANAGERS] = {0};
static uint32_t g_manager_count = 0;
static uint64_t g_vuln_id_counter = 1;
static uint64_t g_scan_id_counter = 1;

int sh_create_hardening_manager(const char *name, HardeningLevel level)
{
    if (!name || g_manager_count >= MAX_HARDENING_MANAGERS)
    {
        return -1;
    }

    SystemHardening *manager = &g_managers[g_manager_count];
    manager->hardening_id = g_manager_count + 1;
    strncpy(manager->name, name, 62);

    manager->current_level = level;
    manager->patch_count = 0;
    manager->vulnerability_count = 0;
    manager->baseline_count = 0;
    manager->scan_count = 0;
    manager->remediation_count = 0;

    manager->total_vulnerabilities_found = 0;
    manager->total_vulnerabilities_remediated = 0;
    manager->patches_applied = 0;
    manager->patches_pending = 0;
    manager->compliance_percentage = 100.0;
    manager->last_scan = 0;
    manager->last_patch = 0;
    manager->auto_patching_enabled = 0;
    manager->is_active = 1;

    g_manager_count++;
    return manager->hardening_id;
}

int sh_delete_hardening_manager(uint32_t hardening_id)
{
    if (hardening_id == 0 || hardening_id > g_manager_count)
    {
        return -1;
    }

    for (uint32_t i = hardening_id - 1; i < g_manager_count - 1; i++)
    {
        g_managers[i] = g_managers[i + 1];
    }
    g_manager_count--;

    return 0;
}

int sh_set_hardening_level(uint32_t hardening_id, HardeningLevel level)
{
    if (hardening_id == 0 || hardening_id > g_manager_count)
    {
        return -1;
    }

    g_managers[hardening_id - 1].current_level = level;
    return 0;
}

int sh_add_security_patch(uint32_t hardening_id, const char *patch_name,
                          const char *cve_id, VulnerabilitySeverity severity)
{
    if (hardening_id == 0 || hardening_id > g_manager_count || !patch_name || !cve_id)
    {
        return -1;
    }

    SystemHardening *manager = &g_managers[hardening_id - 1];
    if (manager->patch_count >= MAX_SECURITY_PATCHES)
    {
        return -1;
    }

    SecurityPatch *patch = &manager->patches[manager->patch_count];
    patch->patch_id = manager->patch_count + 1;
    strncpy(patch->patch_name, patch_name, 126);
    strncpy(patch->cve_id, cve_id, 30);
    patch->severity = severity;
    patch->status = PATCH_STATUS_AVAILABLE;
    strcpy(patch->description, "");
    patch->patch_size_bytes = 1024 * 100;
    patch->release_date = time(NULL);
    patch->applied_date = 0;
    patch->systems_affected = 1;
    patch->systems_patched = 0;
    patch->requires_reboot = (severity >= VULNERABILITY_SEVERITY_HIGH) ? 1 : 0;

    manager->patch_count++;
    manager->patches_pending++;

    return patch->patch_id;
}

int sh_apply_security_patch(uint32_t hardening_id, uint32_t patch_id)
{
    if (hardening_id == 0 || hardening_id > g_manager_count)
    {
        return -1;
    }

    SystemHardening *manager = &g_managers[hardening_id - 1];

    for (uint32_t i = 0; i < manager->patch_count; i++)
    {
        if (manager->patches[i].patch_id == patch_id)
        {
            manager->patches[i].status = PATCH_STATUS_APPLIED;
            manager->patches[i].applied_date = time(NULL);
            manager->patches[i].systems_patched++;
            manager->patches_applied++;
            if (manager->patches_pending > 0)
                manager->patches_pending--;
            manager->last_patch = time(NULL);
            return 0;
        }
    }

    return -1;
}

int sh_stage_patches_for_update(uint32_t hardening_id)
{
    if (hardening_id == 0 || hardening_id > g_manager_count)
    {
        return -1;
    }

    SystemHardening *manager = &g_managers[hardening_id - 1];

    for (uint32_t i = 0; i < manager->patch_count; i++)
    {
        if (manager->patches[i].status == PATCH_STATUS_AVAILABLE)
        {
            manager->patches[i].status = PATCH_STATUS_STAGED;
        }
    }

    return 0;
}

int sh_scan_for_vulnerabilities(uint32_t hardening_id, ScanType scan_type)
{
    if (hardening_id == 0 || hardening_id > g_manager_count)
    {
        return -1;
    }

    SystemHardening *manager = &g_managers[hardening_id - 1];
    if (manager->scan_count >= MAX_VULNERABILITY_SCANS)
    {
        return -1;
    }

    VulnerabilityScan *scan = &manager->scans[manager->scan_count];
    scan->scan_id = g_scan_id_counter++;
    scan->scan_type = scan_type;
    snprintf(scan->scan_name, 126, "SCAN_%lld", (long long)scan->scan_id);
    scan->vulnerabilities_found = 0;
    scan->critical_count = 0;
    scan->high_count = 0;
    scan->medium_count = 0;
    scan->low_count = 0;
    scan->issues_remediated = 0;
    scan->scan_started = time(NULL);
    scan->scan_completed = 0;
    scan->scan_duration_minutes = 0;
    scan->is_automated = 1;

    manager->scan_count++;
    manager->last_scan = time(NULL);

    return scan->scan_id;
}

int sh_discover_vulnerability(uint32_t hardening_id, const char *vuln_name,
                              const char *cve_id, VulnerabilitySeverity severity,
                              const char *affected_component)
{
    if (hardening_id == 0 || hardening_id > g_manager_count || !vuln_name || !cve_id)
    {
        return -1;
    }

    SystemHardening *manager = &g_managers[hardening_id - 1];
    if (manager->vulnerability_count >= MAX_VULNERABILITY_SCANS)
    {
        return -1;
    }

    Vulnerability *vuln = &manager->vulnerabilities[manager->vulnerability_count];
    vuln->vuln_id = g_vuln_id_counter++;
    strncpy(vuln->vuln_name, vuln_name, 126);
    strncpy(vuln->cve_id, cve_id, 30);
    vuln->severity = severity;
    if (affected_component)
        strncpy(vuln->affected_component, affected_component, 126);
    strcpy(vuln->remediation, "");
    vuln->discovered_at = time(NULL);
    vuln->remediated_at = 0;
    vuln->remediation_time_minutes = 0;
    vuln->is_exploitable = (severity >= VULNERABILITY_SEVERITY_HIGH) ? 1 : 0;
    vuln->exploit_probability = 30 + (severity * 20);

    manager->vulnerability_count++;
    manager->total_vulnerabilities_found++;
    manager->compliance_percentage = 100.0 - (manager->vulnerability_count * 5);

    return vuln->vuln_id;
}

int sh_remediate_vulnerability(uint32_t hardening_id, uint64_t vuln_id,
                               RemediationType remediation_type)
{
    if (hardening_id == 0 || hardening_id > g_manager_count)
    {
        return -1;
    }

    SystemHardening *manager = &g_managers[hardening_id - 1];

    for (uint32_t i = 0; i < manager->vulnerability_count; i++)
    {
        if (manager->vulnerabilities[i].vuln_id == vuln_id)
        {
            manager->vulnerabilities[i].remediated_at = time(NULL);
            manager->vulnerabilities[i].remediation_time_minutes =
                (uint32_t)((manager->vulnerabilities[i].remediated_at - manager->vulnerabilities[i].discovered_at) / 60);

            manager->total_vulnerabilities_remediated++;
            manager->compliance_percentage = 100.0 - ((manager->vulnerability_count - manager->total_vulnerabilities_remediated) * 5);

            RemediationAction *action = &manager->remediation_actions[manager->remediation_count];
            if (manager->remediation_count < MAX_REMEDIATION_ACTIONS)
            {
                action->action_id = manager->remediation_count + 1;
                action->action_type = remediation_type;
                snprintf(action->action_description, 254, "Remediation for vuln %lld", (long long)vuln_id);
                strcpy(action->target_resource, "");
                action->execution_time = time(NULL);
                action->was_successful = 1;
                strcpy(action->result_message, "Success");

                manager->remediation_count++;
            }

            return 0;
        }
    }

    return -1;
}

int sh_create_security_baseline(uint32_t hardening_id, const char *baseline_name,
                                HardeningLevel hardening_level)
{
    if (hardening_id == 0 || hardening_id > g_manager_count || !baseline_name)
    {
        return -1;
    }

    SystemHardening *manager = &g_managers[hardening_id - 1];
    if (manager->baseline_count >= MAX_SECURITY_BASELINES)
    {
        return -1;
    }

    SecurityBaseline *baseline = &manager->baselines[manager->baseline_count];
    baseline->baseline_id = manager->baseline_count + 1;
    strncpy(baseline->baseline_name, baseline_name, 62);
    baseline->hardening_level = hardening_level;
    baseline->required_patches = 10 + (hardening_level * 5);
    baseline->security_settings_count = 50 + (hardening_level * 20);
    baseline->disabled_services = 5 + (hardening_level * 3);
    baseline->enforcement_rules = 20 + (hardening_level * 10);
    baseline->created_at = time(NULL);
    baseline->last_verified = 0;
    baseline->is_compliant = 0;

    manager->baseline_count++;

    return baseline->baseline_id;
}

int sh_verify_baseline_compliance(uint32_t hardening_id, uint32_t baseline_id)
{
    if (hardening_id == 0 || hardening_id > g_manager_count)
    {
        return -1;
    }

    SystemHardening *manager = &g_managers[hardening_id - 1];

    for (uint32_t i = 0; i < manager->baseline_count; i++)
    {
        if (manager->baselines[i].baseline_id == baseline_id)
        {
            manager->baselines[i].last_verified = time(NULL);
            manager->baselines[i].is_compliant = 1;
            return 0;
        }
    }

    return -1;
}

int sh_add_remediation_action(uint32_t hardening_id, RemediationType action_type,
                              const char *action_desc, const char *target_resource)
{
    if (hardening_id == 0 || hardening_id > g_manager_count || !action_desc || !target_resource)
    {
        return -1;
    }

    SystemHardening *manager = &g_managers[hardening_id - 1];
    if (manager->remediation_count >= MAX_REMEDIATION_ACTIONS)
    {
        return -1;
    }

    RemediationAction *action = &manager->remediation_actions[manager->remediation_count];
    action->action_id = manager->remediation_count + 1;
    action->action_type = action_type;
    strncpy(action->action_description, action_desc, 254);
    strncpy(action->target_resource, target_resource, 126);
    action->execution_time = time(NULL);
    action->was_successful = 1;
    strcpy(action->result_message, "");

    manager->remediation_count++;

    return action->action_id;
}

int sh_get_vulnerabilities(uint32_t hardening_id, Vulnerability *vulns, uint32_t *count)
{
    if (hardening_id == 0 || hardening_id > g_manager_count || !vulns || !count)
    {
        return -1;
    }

    SystemHardening *manager = &g_managers[hardening_id - 1];

    if (manager->vulnerability_count > *count)
    {
        return -1;
    }

    memcpy(vulns, manager->vulnerabilities, manager->vulnerability_count * sizeof(Vulnerability));
    *count = manager->vulnerability_count;

    return 0;
}

int sh_get_security_patches(uint32_t hardening_id, SecurityPatch *patches, uint32_t *count)
{
    if (hardening_id == 0 || hardening_id > g_manager_count || !patches || !count)
    {
        return -1;
    }

    SystemHardening *manager = &g_managers[hardening_id - 1];

    if (manager->patch_count > *count)
    {
        return -1;
    }

    memcpy(patches, manager->patches, manager->patch_count * sizeof(SecurityPatch));
    *count = manager->patch_count;

    return 0;
}

int sh_get_scan_results(uint32_t hardening_id, VulnerabilityScan *scans, uint32_t *count)
{
    if (hardening_id == 0 || hardening_id > g_manager_count || !scans || !count)
    {
        return -1;
    }

    SystemHardening *manager = &g_managers[hardening_id - 1];

    if (manager->scan_count > *count)
    {
        return -1;
    }

    memcpy(scans, manager->scans, manager->scan_count * sizeof(VulnerabilityScan));
    *count = manager->scan_count;

    return 0;
}

uint64_t sh_get_total_vulnerabilities(uint32_t hardening_id)
{
    if (hardening_id == 0 || hardening_id > g_manager_count)
    {
        return 0;
    }

    return g_managers[hardening_id - 1].total_vulnerabilities_found;
}

uint64_t sh_get_vulnerabilities_remediated(uint32_t hardening_id)
{
    if (hardening_id == 0 || hardening_id > g_manager_count)
    {
        return 0;
    }

    return g_managers[hardening_id - 1].total_vulnerabilities_remediated;
}

uint64_t sh_get_patches_applied(uint32_t hardening_id)
{
    if (hardening_id == 0 || hardening_id > g_manager_count)
    {
        return 0;
    }

    return g_managers[hardening_id - 1].patches_applied;
}

uint64_t sh_get_patches_pending(uint32_t hardening_id)
{
    if (hardening_id == 0 || hardening_id > g_manager_count)
    {
        return 0;
    }

    return g_managers[hardening_id - 1].patches_pending;
}

double sh_get_compliance_percentage(uint32_t hardening_id)
{
    if (hardening_id == 0 || hardening_id > g_manager_count)
    {
        return 0.0;
    }

    return g_managers[hardening_id - 1].compliance_percentage;
}

int sh_enable_auto_patching(uint32_t hardening_id)
{
    if (hardening_id == 0 || hardening_id > g_manager_count)
    {
        return -1;
    }

    g_managers[hardening_id - 1].auto_patching_enabled = 1;
    return 0;
}

int sh_disable_auto_patching(uint32_t hardening_id)
{
    if (hardening_id == 0 || hardening_id > g_manager_count)
    {
        return -1;
    }

    g_managers[hardening_id - 1].auto_patching_enabled = 0;
    return 0;
}

int sh_generate_hardening_report(uint32_t hardening_id, const char *output_path)
{
    if (hardening_id == 0 || hardening_id > g_manager_count || !output_path)
    {
        return -1;
    }

    return 0;
}

int sh_export_hardening_config(uint32_t hardening_id, const char *export_path)
{
    if (hardening_id == 0 || hardening_id > g_manager_count || !export_path)
    {
        return -1;
    }

    return 0;
}

SystemHardening *sh_get_hardening_info(uint32_t hardening_id)
{
    if (hardening_id == 0 || hardening_id > g_manager_count)
    {
        return NULL;
    }

    return &g_managers[hardening_id - 1];
}
