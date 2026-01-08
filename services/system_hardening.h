#ifndef SYSTEM_HARDENING_H
#define SYSTEM_HARDENING_H

#include <stdint.h>
#include <time.h>

#define MAX_HARDENING_MANAGERS 16
#define MAX_HARDENING_POLICIES 256
#define MAX_SECURITY_PATCHES 512
#define MAX_VULNERABILITY_SCANS 1024
#define MAX_REMEDIATION_ACTIONS 2048
#define MAX_SECURITY_BASELINES 128

typedef enum
{
    HARDENING_LEVEL_MINIMAL = 0,
    HARDENING_LEVEL_STANDARD = 1,
    HARDENING_LEVEL_HIGH = 2,
    HARDENING_LEVEL_CRITICAL = 3
} HardeningLevel;

typedef enum
{
    PATCH_STATUS_AVAILABLE = 0,
    PATCH_STATUS_STAGED = 1,
    PATCH_STATUS_APPLIED = 2,
    PATCH_STATUS_FAILED = 3,
    PATCH_STATUS_REJECTED = 4
} PatchStatus;

typedef enum
{
    VULNERABILITY_SEVERITY_LOW = 0,
    VULNERABILITY_SEVERITY_MEDIUM = 1,
    VULNERABILITY_SEVERITY_HIGH = 2,
    VULNERABILITY_SEVERITY_CRITICAL = 3
} VulnerabilitySeverity;

typedef enum
{
    REMEDIATION_TYPE_PATCH = 0,
    REMEDIATION_TYPE_CONFIG_CHANGE = 1,
    REMEDIATION_TYPE_DISABLE_SERVICE = 2,
    REMEDIATION_TYPE_PRIVILEGE_RESTRICTION = 3,
    REMEDIATION_TYPE_ISOLATION = 4
} RemediationType;

typedef enum
{
    SCAN_TYPE_VULNERABILITY = 0,
    SCAN_TYPE_COMPLIANCE = 1,
    SCAN_TYPE_CONFIGURATION = 2,
    SCAN_TYPE_CREDENTIAL = 3
} ScanType;

typedef struct
{
    uint32_t patch_id;
    char patch_name[127];
    char cve_id[31];
    VulnerabilitySeverity severity;
    PatchStatus status;
    char description[255];
    uint64_t patch_size_bytes;
    time_t release_date;
    time_t applied_date;
    uint32_t systems_affected;
    uint32_t systems_patched;
    uint8_t requires_reboot;
} SecurityPatch;

typedef struct
{
    uint64_t vuln_id;
    char vuln_name[127];
    char cve_id[31];
    VulnerabilitySeverity severity;
    char affected_component[127];
    char remediation[255];
    time_t discovered_at;
    time_t remediated_at;
    uint32_t remediation_time_minutes;
    uint8_t is_exploitable;
    uint16_t exploit_probability;
} Vulnerability;

typedef struct
{
    uint32_t baseline_id;
    char baseline_name[63];
    HardeningLevel hardening_level;
    uint32_t required_patches;
    uint32_t security_settings_count;
    uint32_t disabled_services;
    uint32_t enforcement_rules;
    time_t created_at;
    time_t last_verified;
    uint8_t is_compliant;
} SecurityBaseline;

typedef struct
{
    uint32_t action_id;
    RemediationType action_type;
    char action_description[255];
    char target_resource[127];
    time_t execution_time;
    uint8_t was_successful;
    char result_message[255];
} RemediationAction;

typedef struct
{
    uint64_t scan_id;
    ScanType scan_type;
    char scan_name[127];
    uint32_t vulnerabilities_found;
    uint32_t critical_count;
    uint32_t high_count;
    uint32_t medium_count;
    uint32_t low_count;
    uint32_t issues_remediated;
    time_t scan_started;
    time_t scan_completed;
    uint16_t scan_duration_minutes;
    uint8_t is_automated;
} VulnerabilityScan;

typedef struct
{
    uint32_t hardening_id;
    char name[63];
    HardeningLevel current_level;
    uint32_t patch_count;
    uint32_t vulnerability_count;
    uint32_t baseline_count;
    uint32_t scan_count;
    uint32_t remediation_count;
    uint64_t total_vulnerabilities_found;
    uint64_t total_vulnerabilities_remediated;
    uint64_t patches_applied;
    uint64_t patches_pending;
    double compliance_percentage;
    time_t last_scan;
    time_t last_patch;
    uint8_t auto_patching_enabled;
    uint8_t is_active;
    SecurityPatch patches[MAX_SECURITY_PATCHES];
    Vulnerability vulnerabilities[MAX_VULNERABILITY_SCANS];
    SecurityBaseline baselines[MAX_SECURITY_BASELINES];
    RemediationAction remediation_actions[MAX_REMEDIATION_ACTIONS];
    VulnerabilityScan scans[MAX_VULNERABILITY_SCANS];
} SystemHardening;

int sh_create_hardening_manager(const char *name, HardeningLevel level);
int sh_delete_hardening_manager(uint32_t hardening_id);
int sh_set_hardening_level(uint32_t hardening_id, HardeningLevel level);
int sh_add_security_patch(uint32_t hardening_id, const char *patch_name,
                          const char *cve_id, VulnerabilitySeverity severity);
int sh_apply_security_patch(uint32_t hardening_id, uint32_t patch_id);
int sh_stage_patches_for_update(uint32_t hardening_id);
int sh_scan_for_vulnerabilities(uint32_t hardening_id, ScanType scan_type);
int sh_discover_vulnerability(uint32_t hardening_id, const char *vuln_name,
                              const char *cve_id, VulnerabilitySeverity severity,
                              const char *affected_component);
int sh_remediate_vulnerability(uint32_t hardening_id, uint64_t vuln_id,
                               RemediationType remediation_type);
int sh_create_security_baseline(uint32_t hardening_id, const char *baseline_name,
                                HardeningLevel hardening_level);
int sh_verify_baseline_compliance(uint32_t hardening_id, uint32_t baseline_id);
int sh_add_remediation_action(uint32_t hardening_id, RemediationType action_type,
                              const char *action_desc, const char *target_resource);
int sh_get_vulnerabilities(uint32_t hardening_id, Vulnerability *vulns, uint32_t *count);
int sh_get_security_patches(uint32_t hardening_id, SecurityPatch *patches, uint32_t *count);
int sh_get_scan_results(uint32_t hardening_id, VulnerabilityScan *scans, uint32_t *count);
uint64_t sh_get_total_vulnerabilities(uint32_t hardening_id);
uint64_t sh_get_vulnerabilities_remediated(uint32_t hardening_id);
uint64_t sh_get_patches_applied(uint32_t hardening_id);
uint64_t sh_get_patches_pending(uint32_t hardening_id);
double sh_get_compliance_percentage(uint32_t hardening_id);
int sh_enable_auto_patching(uint32_t hardening_id);
int sh_disable_auto_patching(uint32_t hardening_id);
int sh_generate_hardening_report(uint32_t hardening_id, const char *output_path);
int sh_export_hardening_config(uint32_t hardening_id, const char *export_path);
SystemHardening *sh_get_hardening_info(uint32_t hardening_id);

#endif // SYSTEM_HARDENING_H
