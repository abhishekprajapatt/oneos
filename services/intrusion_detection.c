#include "intrusion_detection.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static IntrusionDetectionSystem g_sensors[MAX_IDS_SENSORS] = {0};
static uint32_t g_sensor_count = 0;
static uint64_t g_attack_id_counter = 1;
static uint64_t g_activity_id_counter = 1;
static uint64_t g_flow_id_counter = 1;

int ids_create_sensor(const char *name)
{
    if (!name || g_sensor_count >= MAX_IDS_SENSORS)
    {
        return -1;
    }

    IntrusionDetectionSystem *sensor = &g_sensors[g_sensor_count];
    sensor->sensor_id = g_sensor_count + 1;
    strncpy(sensor->name, name, 62);

    sensor->rule_count = 0;
    sensor->signature_count = 0;
    sensor->flow_count = 0;
    sensor->attack_count = 0;
    sensor->activity_count = 0;

    sensor->packets_analyzed = 0;
    sensor->alerts_generated = 0;
    sensor->true_positives = 0;
    sensor->false_positives = 0;
    sensor->detection_accuracy_percent = 95;
    sensor->last_update = time(NULL);
    sensor->is_active = 1;

    g_sensor_count++;
    return sensor->sensor_id;
}

int ids_delete_sensor(uint32_t sensor_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return -1;
    }

    for (uint32_t i = sensor_id - 1; i < g_sensor_count - 1; i++)
    {
        g_sensors[i] = g_sensors[i + 1];
    }
    g_sensor_count--;

    return 0;
}

int ids_add_attack_signature(uint32_t sensor_id, const char *sig_name,
                             AttackType attack_type, AlertLevel alert_level, const char *pattern)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count || !sig_name || !pattern)
    {
        return -1;
    }

    IntrusionDetectionSystem *sensor = &g_sensors[sensor_id - 1];
    if (sensor->signature_count >= MAX_ATTACK_SIGNATURES)
    {
        return -1;
    }

    AttackSignature *sig = &sensor->signatures[sensor->signature_count];
    sig->signature_id = sensor->signature_count + 1;
    strncpy(sig->signature_name, sig_name, 126);
    sig->attack_type = attack_type;
    sig->alert_level = alert_level;
    strncpy(sig->pattern, pattern, 254);
    sig->match_count = 0;
    sig->created_at = time(NULL);
    sig->is_active = 1;

    sensor->signature_count++;

    return sig->signature_id;
}

int ids_remove_attack_signature(uint32_t sensor_id, uint32_t signature_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return -1;
    }

    IntrusionDetectionSystem *sensor = &g_sensors[sensor_id - 1];

    for (uint32_t i = 0; i < sensor->signature_count; i++)
    {
        if (sensor->signatures[i].signature_id == signature_id)
        {
            for (uint32_t j = i; j < sensor->signature_count - 1; j++)
            {
                sensor->signatures[j] = sensor->signatures[j + 1];
            }
            sensor->signature_count--;
            return 0;
        }
    }

    return -1;
}

int ids_add_ids_rule(uint32_t sensor_id, const char *rule_name, IDSRuleType rule_type,
                     const char *rule_expr, AlertLevel alert_level)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count || !rule_name || !rule_expr)
    {
        return -1;
    }

    IntrusionDetectionSystem *sensor = &g_sensors[sensor_id - 1];
    if (sensor->rule_count >= MAX_IDS_RULES)
    {
        return -1;
    }

    IDSRule *rule = &sensor->rules[sensor->rule_count];
    rule->rule_id = sensor->rule_count + 1;
    strncpy(rule->rule_name, rule_name, 126);
    rule->rule_type = rule_type;
    strncpy(rule->rule_expression, rule_expr, 510);
    rule->trigger_count = 0;
    rule->min_alert_level = alert_level;
    rule->flow_timeout_seconds = 300;
    rule->is_enabled = 1;

    sensor->rule_count++;

    return rule->rule_id;
}

int ids_remove_ids_rule(uint32_t sensor_id, uint32_t rule_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return -1;
    }

    IntrusionDetectionSystem *sensor = &g_sensors[sensor_id - 1];

    for (uint32_t i = 0; i < sensor->rule_count; i++)
    {
        if (sensor->rules[i].rule_id == rule_id)
        {
            for (uint32_t j = i; j < sensor->rule_count - 1; j++)
            {
                sensor->rules[j] = sensor->rules[j + 1];
            }
            sensor->rule_count--;
            return 0;
        }
    }

    return -1;
}

int ids_start_monitoring(uint32_t sensor_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return -1;
    }

    g_sensors[sensor_id - 1].is_active = 1;
    return 0;
}

int ids_stop_monitoring(uint32_t sensor_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return -1;
    }

    g_sensors[sensor_id - 1].is_active = 0;
    return 0;
}

int ids_analyze_traffic(uint32_t sensor_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return -1;
    }

    IntrusionDetectionSystem *sensor = &g_sensors[sensor_id - 1];
    sensor->packets_analyzed += 1000;
    sensor->last_update = time(NULL);

    return 0;
}

int ids_create_protocol_flow(uint32_t sensor_id, uint32_t src_ip, uint32_t dst_ip,
                             uint16_t src_port, uint16_t dst_port, ProtocolType protocol)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return -1;
    }

    IntrusionDetectionSystem *sensor = &g_sensors[sensor_id - 1];
    if (sensor->flow_count >= MAX_PROTOCOL_FLOWS)
    {
        return -1;
    }

    ProtocolFlow *flow = &sensor->flows[sensor->flow_count];
    flow->flow_id = g_flow_id_counter++;
    flow->source_ip = src_ip;
    flow->dest_ip = dst_ip;
    flow->source_port = src_port;
    flow->dest_port = dst_port;
    flow->protocol = protocol;
    flow->packets_sent = 0;
    flow->packets_received = 0;
    flow->bytes_sent = 0;
    flow->bytes_received = 0;
    flow->start_time = time(NULL);
    flow->last_packet_time = time(NULL);
    flow->is_suspicious = 0;

    sensor->flow_count++;

    return flow->flow_id;
}

int ids_update_protocol_flow(uint32_t sensor_id, uint64_t flow_id, uint64_t packets,
                             uint64_t bytes, uint8_t suspicious)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return -1;
    }

    IntrusionDetectionSystem *sensor = &g_sensors[sensor_id - 1];

    for (uint32_t i = 0; i < sensor->flow_count; i++)
    {
        if (sensor->flows[i].flow_id == flow_id)
        {
            sensor->flows[i].packets_sent += packets;
            sensor->flows[i].bytes_sent += bytes;
            sensor->flows[i].last_packet_time = time(NULL);
            if (suspicious)
            {
                sensor->flows[i].is_suspicious = 1;
            }
            return 0;
        }
    }

    return -1;
}

int ids_detect_attack(uint32_t sensor_id, AttackType attack_type,
                      uint32_t src_ip, uint32_t dst_ip, uint16_t src_port, uint16_t dst_port,
                      uint32_t signature_id, const char *description)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count || !description)
    {
        return -1;
    }

    IntrusionDetectionSystem *sensor = &g_sensors[sensor_id - 1];
    if (sensor->attack_count >= MAX_DETECTED_ATTACKS)
    {
        return -1;
    }

    DetectedAttack *attack = &sensor->attacks[sensor->attack_count];
    attack->alert_id = g_attack_id_counter++;
    attack->attack_type = attack_type;
    attack->alert_level = ALERT_LEVEL_MEDIUM;
    attack->source_ip = src_ip;
    attack->dest_ip = dst_ip;
    attack->source_port = src_port;
    attack->dest_port = dst_port;
    attack->protocol = PROTOCOL_TYPE_TCP;
    attack->signature_id = signature_id;
    attack->rule_id = 0;
    attack->status = ATTACK_STATUS_DETECTED;
    strncpy(attack->attack_description, description, 254);
    attack->detected_at = time(NULL);
    attack->confirmed_at = 0;
    attack->resolved_at = 0;
    attack->confidence_score = 80 + (attack_type % 20);
    attack->requires_immediate_action = (attack_type >= ATTACK_TYPE_DDoS) ? 1 : 0;

    for (uint32_t i = 0; i < sensor->signature_count; i++)
    {
        if (sensor->signatures[i].signature_id == signature_id)
        {
            attack->alert_level = sensor->signatures[i].alert_level;
            sensor->signatures[i].match_count++;
        }
    }

    sensor->attack_count++;
    sensor->alerts_generated++;

    return attack->alert_id;
}

int ids_confirm_attack(uint32_t sensor_id, uint64_t attack_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return -1;
    }

    IntrusionDetectionSystem *sensor = &g_sensors[sensor_id - 1];

    for (uint32_t i = 0; i < sensor->attack_count; i++)
    {
        if (sensor->attacks[i].alert_id == attack_id)
        {
            sensor->attacks[i].status = ATTACK_STATUS_CONFIRMED;
            sensor->attacks[i].confirmed_at = time(NULL);
            sensor->true_positives++;
            return 0;
        }
    }

    return -1;
}

int ids_mitigate_attack(uint32_t sensor_id, uint64_t attack_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return -1;
    }

    IntrusionDetectionSystem *sensor = &g_sensors[sensor_id - 1];

    for (uint32_t i = 0; i < sensor->attack_count; i++)
    {
        if (sensor->attacks[i].alert_id == attack_id)
        {
            sensor->attacks[i].status = ATTACK_STATUS_MITIGATED;
            sensor->attacks[i].resolved_at = time(NULL);
            return 0;
        }
    }

    return -1;
}

int ids_block_attack(uint32_t sensor_id, uint64_t attack_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return -1;
    }

    IntrusionDetectionSystem *sensor = &g_sensors[sensor_id - 1];

    for (uint32_t i = 0; i < sensor->attack_count; i++)
    {
        if (sensor->attacks[i].alert_id == attack_id)
        {
            sensor->attacks[i].status = ATTACK_STATUS_BLOCKED;
            sensor->attacks[i].resolved_at = time(NULL);
            return 0;
        }
    }

    return -1;
}

int ids_mark_false_positive(uint32_t sensor_id, uint64_t attack_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return -1;
    }

    IntrusionDetectionSystem *sensor = &g_sensors[sensor_id - 1];

    for (uint32_t i = 0; i < sensor->attack_count; i++)
    {
        if (sensor->attacks[i].alert_id == attack_id)
        {
            sensor->attacks[i].status = ATTACK_STATUS_FALSE_POSITIVE;
            sensor->false_positives++;
            return 0;
        }
    }

    return -1;
}

int ids_detect_suspicious_activity(uint32_t sensor_id, uint32_t src_ip,
                                   const char *activity_type, AlertLevel threat_level)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count || !activity_type)
    {
        return -1;
    }

    IntrusionDetectionSystem *sensor = &g_sensors[sensor_id - 1];
    if (sensor->activity_count >= MAX_SUSPICIOUS_ACTIVITIES)
    {
        return -1;
    }

    SuspiciousActivity *activity = &sensor->activities[sensor->activity_count];
    activity->activity_id = g_activity_id_counter++;
    activity->source_ip = src_ip;
    strncpy(activity->activity_type, activity_type, 126);
    strcpy(activity->activity_details, "");
    activity->activity_count = 1;
    activity->first_occurrence = time(NULL);
    activity->last_occurrence = time(NULL);
    activity->threat_level = threat_level;
    activity->is_anomalous = 1;

    sensor->activity_count++;

    return activity->activity_id;
}

int ids_get_detected_attacks(uint32_t sensor_id, DetectedAttack *attacks, uint32_t *count)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count || !attacks || !count)
    {
        return -1;
    }

    IntrusionDetectionSystem *sensor = &g_sensors[sensor_id - 1];

    if (sensor->attack_count > *count)
    {
        return -1;
    }

    memcpy(attacks, sensor->attacks, sensor->attack_count * sizeof(DetectedAttack));
    *count = sensor->attack_count;

    return 0;
}

int ids_get_attack_details(uint32_t sensor_id, uint64_t attack_id, DetectedAttack *attack)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count || !attack)
    {
        return -1;
    }

    IntrusionDetectionSystem *sensor = &g_sensors[sensor_id - 1];

    for (uint32_t i = 0; i < sensor->attack_count; i++)
    {
        if (sensor->attacks[i].alert_id == attack_id)
        {
            memcpy(attack, &sensor->attacks[i], sizeof(DetectedAttack));
            return 0;
        }
    }

    return -1;
}

int ids_get_suspicious_activities(uint32_t sensor_id, SuspiciousActivity *activities, uint32_t *count)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count || !activities || !count)
    {
        return -1;
    }

    IntrusionDetectionSystem *sensor = &g_sensors[sensor_id - 1];

    if (sensor->activity_count > *count)
    {
        return -1;
    }

    memcpy(activities, sensor->activities, sensor->activity_count * sizeof(SuspiciousActivity));
    *count = sensor->activity_count;

    return 0;
}

uint64_t ids_get_total_alerts(uint32_t sensor_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return 0;
    }

    return g_sensors[sensor_id - 1].alerts_generated;
}

uint64_t ids_get_total_attacks_detected(uint32_t sensor_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return 0;
    }

    return g_sensors[sensor_id - 1].attack_count;
}

uint64_t ids_get_true_positives(uint32_t sensor_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return 0;
    }

    return g_sensors[sensor_id - 1].true_positives;
}

uint64_t ids_get_false_positives(uint32_t sensor_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return 0;
    }

    return g_sensors[sensor_id - 1].false_positives;
}

uint64_t ids_get_packets_analyzed(uint32_t sensor_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return 0;
    }

    return g_sensors[sensor_id - 1].packets_analyzed;
}

int ids_generate_ids_report(uint32_t sensor_id, const char *output_path)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count || !output_path)
    {
        return -1;
    }

    return 0;
}

int ids_export_ids_config(uint32_t sensor_id, const char *export_path)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count || !export_path)
    {
        return -1;
    }

    return 0;
}

int ids_update_threat_intelligence(uint32_t sensor_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return -1;
    }

    return 0;
}

int ids_correlate_alerts(uint32_t sensor_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return -1;
    }

    return 0;
}

IntrusionDetectionSystem *ids_get_sensor_info(uint32_t sensor_id)
{
    if (sensor_id == 0 || sensor_id > g_sensor_count)
    {
        return NULL;
    }

    return &g_sensors[sensor_id - 1];
}
