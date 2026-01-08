#ifndef THREAT_DETECTOR_H
#define THREAT_DETECTOR_H

#include <stdint.h>
#include <time.h>

#define MAX_THREAT_DETECTORS 16
#define MAX_THREAT_SIGNATURES 512
#define MAX_ACTIVE_THREATS 1024
#define MAX_THREAT_RULES 256
#define MAX_MALWARE_PATTERNS 2048
#define MAX_BEHAVIORAL_PROFILES 512

typedef enum
{
    THREAT_TYPE_MALWARE = 0,
    THREAT_TYPE_EXPLOIT = 1,
    THREAT_TYPE_ROOTKIT = 2,
    THREAT_TYPE_RANSOMWARE = 3,
    THREAT_TYPE_BOTNET = 4,
    THREAT_TYPE_APT = 5,
    THREAT_TYPE_ZERO_DAY = 6,
    THREAT_TYPE_ANOMALY = 7
} ThreatType;

typedef enum
{
    THREAT_SEVERITY_INFO = 0,
    THREAT_SEVERITY_LOW = 1,
    THREAT_SEVERITY_MEDIUM = 2,
    THREAT_SEVERITY_HIGH = 3,
    THREAT_SEVERITY_CRITICAL = 4
} ThreatSeverity;

typedef enum
{
    THREAT_STATUS_DETECTED = 0,
    THREAT_STATUS_QUARANTINED = 1,
    THREAT_STATUS_CLEANED = 2,
    THREAT_STATUS_BLOCKED = 3,
    THREAT_STATUS_CONTAINED = 4
} ThreatStatus;

typedef enum
{
    THREAT_RULE_TYPE_SIGNATURE = 0,
    THREAT_RULE_TYPE_HEURISTIC = 1,
    THREAT_RULE_TYPE_BEHAVIORAL = 2,
    THREAT_RULE_TYPE_ML_BASED = 3,
    THREAT_RULE_TYPE_SANDBOX = 4
} ThreatRuleType;

typedef enum
{
    DETECTION_ENGINE_SIGNATURE = 0,
    DETECTION_ENGINE_BEHAVIORAL = 1,
    DETECTION_ENGINE_HEURISTIC = 2,
    DETECTION_ENGINE_MACHINE_LEARNING = 3
} DetectionEngine;

typedef struct
{
    uint32_t signature_id;
    char signature_name[127];
    char signature_hash[63];
    ThreatType threat_type;
    ThreatSeverity severity;
    uint32_t detection_count;
    time_t created_at;
    time_t last_detected_at;
    uint8_t is_active;
} ThreatSignature;

typedef struct
{
    uint64_t threat_id;
    char threat_name[127];
    ThreatType threat_type;
    ThreatSeverity severity;
    ThreatStatus status;
    uint32_t signature_id;
    char source_path[255];
    char process_name[127];
    uint32_t affected_resources;
    time_t detected_at;
    time_t resolved_at;
    char quarantine_location[255];
    uint32_t confidence_score;
    uint8_t requires_immediate_action;
} ActiveThreat;

typedef struct
{
    uint32_t rule_id;
    char rule_name[127];
    ThreatRuleType rule_type;
    DetectionEngine engine;
    char rule_expression[512];
    uint32_t match_count;
    uint32_t false_positive_count;
    uint8_t is_enabled;
    ThreatSeverity min_severity;
    uint16_t detection_response_time_ms;
} ThreatRule;

typedef struct
{
    uint32_t pattern_id;
    char malware_family[63];
    char pattern_signature[255];
    ThreatType malware_type;
    uint32_t variant_count;
    uint32_t infection_count;
    time_t first_seen;
    time_t last_seen;
    uint8_t is_current_threat;
    uint16_t malware_score;
} MalwarePattern;

typedef struct
{
    uint32_t profile_id;
    uint32_t process_id;
    char process_name[127];
    uint32_t baseline_cpu_usage;
    uint32_t baseline_memory_usage;
    uint32_t baseline_file_access_count;
    uint32_t baseline_network_calls;
    uint32_t anomaly_count;
    time_t profile_created_at;
    time_t last_updated_at;
    uint8_t is_trusted;
    uint16_t anomaly_score;
} BehavioralProfile;

typedef struct
{
    uint32_t detector_id;
    char name[63];
    uint32_t signature_count;
    uint32_t rule_count;
    uint32_t pattern_count;
    uint32_t profile_count;
    uint32_t active_threat_count;
    uint64_t total_threats_detected;
    uint64_t total_threats_quarantined;
    uint64_t total_threats_cleaned;
    uint64_t false_positive_count;
    uint64_t true_positive_count;
    uint32_t detection_engine_mask;
    time_t last_signature_update;
    time_t last_scan_time;
    uint32_t threat_db_version;
    uint8_t real_time_scanning_enabled;
    uint8_t sandbox_analysis_enabled;
    uint8_t machine_learning_enabled;
    ThreatSignature signatures[MAX_THREAT_SIGNATURES];
    ActiveThreat active_threats[MAX_ACTIVE_THREATS];
    ThreatRule rules[MAX_THREAT_RULES];
    MalwarePattern patterns[MAX_MALWARE_PATTERNS];
    BehavioralProfile profiles[MAX_BEHAVIORAL_PROFILES];
} ThreatDetector;

int td_create_detector(const char *name);
int td_delete_detector(uint32_t detector_id);
int td_add_threat_signature(uint32_t detector_id, const char *sig_name, const char *sig_hash,
                            ThreatType threat_type, ThreatSeverity severity);
int td_remove_threat_signature(uint32_t detector_id, uint32_t signature_id);
int td_scan_system(uint32_t detector_id);
int td_scan_file(uint32_t detector_id, const char *file_path);
int td_scan_process(uint32_t detector_id, uint32_t process_id);
int td_detect_threat(uint32_t detector_id, const char *threat_name, ThreatType threat_type,
                     const char *source_path, uint32_t signature_id);
int td_quarantine_threat(uint32_t detector_id, uint64_t threat_id, const char *quarantine_path);
int td_remove_threat(uint32_t detector_id, uint64_t threat_id);
int td_add_threat_rule(uint32_t detector_id, const char *rule_name, ThreatRuleType rule_type,
                       DetectionEngine engine, const char *rule_expression);
int td_add_malware_pattern(uint32_t detector_id, const char *malware_family,
                           const char *pattern_sig, ThreatType malware_type);
int td_create_behavioral_profile(uint32_t detector_id, uint32_t process_id, const char *process_name);
int td_update_behavioral_profile(uint32_t detector_id, uint32_t profile_id,
                                 uint32_t cpu_usage, uint32_t mem_usage, uint32_t file_access);
int td_detect_behavioral_anomaly(uint32_t detector_id, uint32_t profile_id);
int td_update_threat_database(uint32_t detector_id);
int td_enable_real_time_scanning(uint32_t detector_id);
int td_disable_real_time_scanning(uint32_t detector_id);
int td_enable_sandbox_analysis(uint32_t detector_id);
int td_enable_machine_learning(uint32_t detector_id);
int td_get_active_threats(uint32_t detector_id, ActiveThreat *threats, uint32_t *count);
int td_get_threat_details(uint32_t detector_id, uint64_t threat_id, ActiveThreat *threat);
uint64_t td_get_total_threats_detected(uint32_t detector_id);
uint64_t td_get_total_threats_quarantined(uint32_t detector_id);
uint64_t td_get_total_false_positives(uint32_t detector_id);
uint64_t td_get_total_true_positives(uint32_t detector_id);
int td_get_threat_intelligence(uint32_t detector_id, const char *query);
int td_generate_threat_report(uint32_t detector_id, const char *output_path);
ThreatDetector *td_get_detector_info(uint32_t detector_id);

#endif // THREAT_DETECTOR_H
