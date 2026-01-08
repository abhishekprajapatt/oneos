#ifndef INTRUSION_DETECTION_H
#define INTRUSION_DETECTION_H

#include <stdint.h>
#include <time.h>

#define MAX_IDS_SENSORS 32
#define MAX_IDS_RULES 1024
#define MAX_DETECTED_ATTACKS 2048
#define MAX_ATTACK_SIGNATURES 512
#define MAX_PROTOCOL_FLOWS 4096
#define MAX_SUSPICIOUS_ACTIVITIES 2048

typedef enum
{
    ATTACK_TYPE_RECONNAISSANCE = 0,
    ATTACK_TYPE_PORT_SCAN = 1,
    ATTACK_TYPE_SQL_INJECTION = 2,
    ATTACK_TYPE_XSS_ATTACK = 3,
    ATTACK_TYPE_DDoS = 4,
    ATTACK_TYPE_BUFFER_OVERFLOW = 5,
    ATTACK_TYPE_PRIVILEGE_ESCALATION = 6,
    ATTACK_TYPE_DATA_EXFILTRATION = 7,
    ATTACK_TYPE_LATERAL_MOVEMENT = 8,
    ATTACK_TYPE_C2_COMMUNICATION = 9
} AttackType;

typedef enum
{
    ALERT_LEVEL_LOW = 0,
    ALERT_LEVEL_MEDIUM = 1,
    ALERT_LEVEL_HIGH = 2,
    ALERT_LEVEL_CRITICAL = 3
} AlertLevel;

typedef enum
{
    IDS_RULE_TYPE_SIGNATURE = 0,
    IDS_RULE_TYPE_ANOMALY = 1,
    IDS_RULE_TYPE_STATEFUL = 2,
    IDS_RULE_TYPE_STATISTICAL = 3
} IDSRuleType;

typedef enum
{
    PROTOCOL_TYPE_TCP = 0,
    PROTOCOL_TYPE_UDP = 1,
    PROTOCOL_TYPE_ICMP = 2,
    PROTOCOL_TYPE_HTTP = 3,
    PROTOCOL_TYPE_HTTPS = 4,
    PROTOCOL_TYPE_DNS = 5
} ProtocolType;

typedef enum
{
    ATTACK_STATUS_DETECTED = 0,
    ATTACK_STATUS_CONFIRMED = 1,
    ATTACK_STATUS_MITIGATED = 2,
    ATTACK_STATUS_BLOCKED = 3,
    ATTACK_STATUS_FALSE_POSITIVE = 4
} AttackStatus;

typedef struct
{
    uint32_t signature_id;
    char signature_name[127];
    AttackType attack_type;
    AlertLevel alert_level;
    char pattern[255];
    uint32_t match_count;
    time_t created_at;
    uint8_t is_active;
} AttackSignature;

typedef struct
{
    uint32_t rule_id;
    char rule_name[127];
    IDSRuleType rule_type;
    char rule_expression[511];
    uint32_t trigger_count;
    AlertLevel min_alert_level;
    uint32_t flow_timeout_seconds;
    uint8_t is_enabled;
} IDSRule;

typedef struct
{
    uint64_t flow_id;
    uint32_t source_ip;
    uint32_t dest_ip;
    uint16_t source_port;
    uint16_t dest_port;
    ProtocolType protocol;
    uint64_t packets_sent;
    uint64_t packets_received;
    uint64_t bytes_sent;
    uint64_t bytes_received;
    time_t start_time;
    time_t last_packet_time;
    uint8_t is_suspicious;
} ProtocolFlow;

typedef struct
{
    uint64_t alert_id;
    AttackType attack_type;
    AlertLevel alert_level;
    uint32_t source_ip;
    uint32_t dest_ip;
    uint16_t source_port;
    uint16_t dest_port;
    ProtocolType protocol;
    uint32_t signature_id;
    uint32_t rule_id;
    AttackStatus status;
    char attack_description[255];
    time_t detected_at;
    time_t confirmed_at;
    time_t resolved_at;
    uint32_t confidence_score;
    uint8_t requires_immediate_action;
} DetectedAttack;

typedef struct
{
    uint64_t activity_id;
    uint32_t source_ip;
    char activity_type[127];
    char activity_details[255];
    uint16_t activity_count;
    time_t first_occurrence;
    time_t last_occurrence;
    AlertLevel threat_level;
    uint8_t is_anomalous;
} SuspiciousActivity;

typedef struct
{
    uint32_t sensor_id;
    char name[63];
    uint32_t rule_count;
    uint32_t signature_count;
    uint32_t flow_count;
    uint32_t attack_count;
    uint32_t activity_count;
    uint64_t packets_analyzed;
    uint64_t alerts_generated;
    uint64_t true_positives;
    uint64_t false_positives;
    uint32_t detection_accuracy_percent;
    time_t last_update;
    uint8_t is_active;
    IDSRule rules[MAX_IDS_RULES];
    AttackSignature signatures[MAX_ATTACK_SIGNATURES];
    ProtocolFlow flows[MAX_PROTOCOL_FLOWS];
    DetectedAttack attacks[MAX_DETECTED_ATTACKS];
    SuspiciousActivity activities[MAX_SUSPICIOUS_ACTIVITIES];
} IntrusionDetectionSystem;

int ids_create_sensor(const char *name);
int ids_delete_sensor(uint32_t sensor_id);
int ids_add_attack_signature(uint32_t sensor_id, const char *sig_name,
                             AttackType attack_type, AlertLevel alert_level, const char *pattern);
int ids_remove_attack_signature(uint32_t sensor_id, uint32_t signature_id);
int ids_add_ids_rule(uint32_t sensor_id, const char *rule_name, IDSRuleType rule_type,
                     const char *rule_expr, AlertLevel alert_level);
int ids_remove_ids_rule(uint32_t sensor_id, uint32_t rule_id);
int ids_start_monitoring(uint32_t sensor_id);
int ids_stop_monitoring(uint32_t sensor_id);
int ids_analyze_traffic(uint32_t sensor_id);
int ids_create_protocol_flow(uint32_t sensor_id, uint32_t src_ip, uint32_t dst_ip,
                             uint16_t src_port, uint16_t dst_port, ProtocolType protocol);
int ids_update_protocol_flow(uint32_t sensor_id, uint64_t flow_id, uint64_t packets,
                             uint64_t bytes, uint8_t suspicious);
int ids_detect_attack(uint32_t sensor_id, AttackType attack_type,
                      uint32_t src_ip, uint32_t dst_ip, uint16_t src_port, uint16_t dst_port,
                      uint32_t signature_id, const char *description);
int ids_confirm_attack(uint32_t sensor_id, uint64_t attack_id);
int ids_mitigate_attack(uint32_t sensor_id, uint64_t attack_id);
int ids_block_attack(uint32_t sensor_id, uint64_t attack_id);
int ids_mark_false_positive(uint32_t sensor_id, uint64_t attack_id);
int ids_detect_suspicious_activity(uint32_t sensor_id, uint32_t src_ip,
                                   const char *activity_type, AlertLevel threat_level);
int ids_get_detected_attacks(uint32_t sensor_id, DetectedAttack *attacks, uint32_t *count);
int ids_get_attack_details(uint32_t sensor_id, uint64_t attack_id, DetectedAttack *attack);
int ids_get_suspicious_activities(uint32_t sensor_id, SuspiciousActivity *activities, uint32_t *count);
uint64_t ids_get_total_alerts(uint32_t sensor_id);
uint64_t ids_get_total_attacks_detected(uint32_t sensor_id);
uint64_t ids_get_true_positives(uint32_t sensor_id);
uint64_t ids_get_false_positives(uint32_t sensor_id);
uint64_t ids_get_packets_analyzed(uint32_t sensor_id);
int ids_generate_ids_report(uint32_t sensor_id, const char *output_path);
int ids_export_ids_config(uint32_t sensor_id, const char *export_path);
int ids_update_threat_intelligence(uint32_t sensor_id);
int ids_correlate_alerts(uint32_t sensor_id);
IntrusionDetectionSystem *ids_get_sensor_info(uint32_t sensor_id);

#endif // INTRUSION_DETECTION_H
