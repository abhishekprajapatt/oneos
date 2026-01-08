#include "network_security.h"
#include <stdlib.h>
#include <string.h>

static NetworkSecurity *global_security = NULL;
static uint32_t next_rule_id = 1;
static uint32_t next_cert_id = 1;
static uint32_t next_alert_id = 1;

NetworkSecurity *network_security_init(void)
{
    NetworkSecurity *sec = (NetworkSecurity *)malloc(sizeof(NetworkSecurity));
    if (!sec)
        return NULL;

    memset(sec, 0, sizeof(NetworkSecurity));
    sec->security_id = 1;
    sec->firewall_rule_count = 0;
    sec->certificate_count = 0;
    sec->cipher_suite_count = 0;
    sec->intrusion_alert_count = 0;
    sec->total_packets_inspected = 0;
    sec->total_packets_blocked = 0;
    sec->total_threats_detected = 0;
    sec->ddos_protection_enabled = 1;
    sec->intrusion_detection_enabled = 1;
    sec->ssl_tls_enabled = 1;

    global_security = sec;
    return sec;
}

int network_security_destroy(NetworkSecurity *sec)
{
    if (!sec)
        return -1;

    for (uint32_t i = 0; i < sec->certificate_count; i++)
    {
        if (sec->certificates[i].cert_data)
            free(sec->certificates[i].cert_data);
    }

    free(sec);
    global_security = NULL;
    return 0;
}

int security_add_firewall_rule(NetworkSecurity *sec, const char *src_ip, const char *dst_ip,
                               uint32_t src_port, uint32_t dst_port, const char *protocol,
                               FirewallAction action)
{
    if (!sec || sec->firewall_rule_count >= MAX_FIREWALL_RULES || !src_ip || !dst_ip || !protocol)
        return -1;

    FirewallRule *rule = &sec->firewall_rules[sec->firewall_rule_count];
    memset(rule, 0, sizeof(FirewallRule));

    rule->rule_id = next_rule_id++;
    rule->priority = sec->firewall_rule_count;
    strncpy(rule->source_ip, src_ip, 63);
    strncpy(rule->destination_ip, dst_ip, 63);
    rule->source_port = src_port;
    rule->destination_port = dst_port;
    strncpy(rule->protocol, protocol, 7);
    rule->direction = RULE_DIRECTION_BIDIRECTIONAL;
    rule->action = action;
    rule->packets_matched = 0;
    rule->bytes_matched = 0;
    rule->enabled = 1;

    sec->firewall_rule_count++;
    return 0;
}

int security_remove_firewall_rule(NetworkSecurity *sec, uint32_t rule_id)
{
    if (!sec)
        return -1;

    for (uint32_t i = 0; i < sec->firewall_rule_count; i++)
    {
        if (sec->firewall_rules[i].rule_id == rule_id)
        {
            sec->firewall_rules[i].enabled = 0;
            return 0;
        }
    }

    return -1;
}

int security_check_firewall_rule(NetworkSecurity *sec, const char *src_ip, const char *dst_ip,
                                 const char *protocol)
{
    if (!sec || !src_ip || !dst_ip || !protocol)
        return -1;

    for (uint32_t i = 0; i < sec->firewall_rule_count; i++)
    {
        FirewallRule *rule = &sec->firewall_rules[i];

        if (!rule->enabled)
            continue;

        if (strcmp(rule->source_ip, src_ip) == 0 &&
            strcmp(rule->destination_ip, dst_ip) == 0 &&
            strcmp(rule->protocol, protocol) == 0)
        {

            if (rule->action == FIREWALL_ACTION_DROP)
                return -1; // Packet dropped
            else if (rule->action == FIREWALL_ACTION_ACCEPT)
                return 0; // Packet accepted
        }
    }

    return 0; // Default accept
}

int security_enable_firewall(NetworkSecurity *sec)
{
    if (!sec)
        return -1;

    for (uint32_t i = 0; i < sec->firewall_rule_count; i++)
    {
        sec->firewall_rules[i].enabled = 1;
    }

    return 0;
}

int security_disable_firewall(NetworkSecurity *sec)
{
    if (!sec)
        return -1;

    for (uint32_t i = 0; i < sec->firewall_rule_count; i++)
    {
        sec->firewall_rules[i].enabled = 0;
    }

    return 0;
}

int security_add_certificate(NetworkSecurity *sec, const char *cert_name, const uint8_t *cert_data, uint32_t cert_size)
{
    if (!sec || sec->certificate_count >= MAX_CERTIFICATES || !cert_name || !cert_data)
        return -1;

    Certificate *cert = &sec->certificates[sec->certificate_count];
    memset(cert, 0, sizeof(Certificate));

    cert->cert_id = next_cert_id++;
    strncpy(cert->cert_name, cert_name, 63);

    cert->cert_data = (uint8_t *)malloc(cert_size);
    if (!cert->cert_data)
        return -1;

    memcpy(cert->cert_data, cert_data, cert_size);
    cert->cert_size = cert_size;

    strncpy(cert->issuer, "Self-Signed", 127);
    strncpy(cert->subject, cert_name, 127);

    cert->valid_from = 0;
    cert->valid_until = 0xFFFFFFFF;
    cert->is_expired = 0;
    cert->is_revoked = 0;

    sec->certificate_count++;
    return 0;
}

int security_remove_certificate(NetworkSecurity *sec, uint32_t cert_id)
{
    if (!sec)
        return -1;

    for (uint32_t i = 0; i < sec->certificate_count; i++)
    {
        if (sec->certificates[i].cert_id == cert_id)
        {
            if (sec->certificates[i].cert_data)
                free(sec->certificates[i].cert_data);
            return 0;
        }
    }

    return -1;
}

int security_verify_certificate(NetworkSecurity *sec, uint32_t cert_id)
{
    if (!sec)
        return -1;

    for (uint32_t i = 0; i < sec->certificate_count; i++)
    {
        if (sec->certificates[i].cert_id == cert_id)
        {
            return (sec->certificates[i].is_expired || sec->certificates[i].is_revoked) ? -1 : 0;
        }
    }

    return -1;
}

int security_revoke_certificate(NetworkSecurity *sec, uint32_t cert_id)
{
    if (!sec)
        return -1;

    for (uint32_t i = 0; i < sec->certificate_count; i++)
    {
        if (sec->certificates[i].cert_id == cert_id)
        {
            sec->certificates[i].is_revoked = 1;
            return 0;
        }
    }

    return -1;
}

int security_add_cipher_suite(NetworkSecurity *sec, CipherSuite cipher, uint32_t key_size)
{
    if (!sec || sec->cipher_suite_count >= MAX_CIPHER_SUITES)
        return -1;

    CipherSuiteConfig *config = &sec->cipher_suites[sec->cipher_suite_count];
    memset(config, 0, sizeof(CipherSuiteConfig));

    config->cipher_suite_id = sec->cipher_suite_count;
    config->cipher_type = cipher;
    config->key_size = key_size;
    config->enabled = 1;
    config->supported = 1;

    sec->cipher_suite_count++;
    return 0;
}

int security_remove_cipher_suite(NetworkSecurity *sec, uint32_t cipher_id)
{
    if (!sec || cipher_id >= sec->cipher_suite_count)
        return -1;

    sec->cipher_suites[cipher_id].enabled = 0;
    return 0;
}

int security_get_supported_ciphers(NetworkSecurity *sec)
{
    if (!sec)
        return -1;

    return sec->cipher_suite_count;
}

int security_enable_ssl_tls(NetworkSecurity *sec)
{
    if (!sec)
        return -1;

    sec->ssl_tls_enabled = 1;
    return 0;
}

int security_disable_ssl_tls(NetworkSecurity *sec)
{
    if (!sec)
        return -1;

    sec->ssl_tls_enabled = 0;
    return 0;
}

int security_set_min_tls_version(NetworkSecurity *sec, uint32_t version)
{
    if (!sec)
        return -1;

    return 0;
}

int security_set_max_tls_version(NetworkSecurity *sec, uint32_t version)
{
    if (!sec)
        return -1;

    return 0;
}

int security_enable_ddos_protection(NetworkSecurity *sec)
{
    if (!sec)
        return -1;

    sec->ddos_protection_enabled = 1;
    return 0;
}

int security_disable_ddos_protection(NetworkSecurity *sec)
{
    if (!sec)
        return -1;

    sec->ddos_protection_enabled = 0;
    return 0;
}

int security_detect_ddos_attack(NetworkSecurity *sec)
{
    if (!sec || !sec->ddos_protection_enabled)
        return -1;

    // Analyze traffic patterns for DDoS
    if (sec->total_packets_inspected > 10000)
    {
        sec->total_threats_detected++;
        return 1; // DDoS detected
    }

    return 0;
}

int security_mitigate_ddos(NetworkSecurity *sec)
{
    if (!sec)
        return -1;

    // Drop excess traffic
    sec->total_packets_blocked += 1000;
    return 0;
}

int security_enable_intrusion_detection(NetworkSecurity *sec)
{
    if (!sec)
        return -1;

    sec->intrusion_detection_enabled = 1;
    return 0;
}

int security_disable_intrusion_detection(NetworkSecurity *sec)
{
    if (!sec)
        return -1;

    sec->intrusion_detection_enabled = 0;
    return 0;
}

int security_check_intrusion(NetworkSecurity *sec, const uint8_t *packet, uint32_t size)
{
    if (!sec || !packet || !sec->intrusion_detection_enabled)
        return 0;

    // Simple pattern matching for known attacks
    if (size > 10000)
    {
        return 1; // Suspicious packet size
    }

    return 0;
}

int security_log_intrusion_alert(NetworkSecurity *sec, const char *alert_type, const char *description, uint32_t severity)
{
    if (!sec || sec->intrusion_alert_count >= MAX_INTRUSION_ALERTS || !alert_type)
        return -1;

    IntrusionAlert *alert = &sec->intrusion_alerts[sec->intrusion_alert_count];
    memset(alert, 0, sizeof(IntrusionAlert));

    alert->alert_id = next_alert_id++;
    alert->timestamp = 0;
    strncpy(alert->source_ip, "0.0.0.0", 63);
    strncpy(alert->destination_ip, "0.0.0.0", 63);
    alert->source_port = 0;
    alert->destination_port = 0;
    strncpy(alert->alert_type, alert_type, 63);
    strncpy(alert->description, description, 255);
    alert->severity = severity;
    alert->action_taken = 1;

    sec->intrusion_alert_count++;
    sec->total_threats_detected++;

    return 0;
}

int security_inspect_packet(NetworkSecurity *sec, const uint8_t *packet, uint32_t size)
{
    if (!sec || !packet)
        return -1;

    sec->total_packets_inspected++;
    return 0;
}

int security_block_packet(NetworkSecurity *sec)
{
    if (!sec)
        return -1;

    sec->total_packets_blocked++;
    return 0;
}

int security_allow_packet(NetworkSecurity *sec)
{
    if (!sec)
        return -1;

    return 0;
}

int security_get_metrics(NetworkSecurity *sec, NetworkSecurityMetrics *metrics)
{
    if (!sec || !metrics)
        return -1;

    metrics->security_id = sec->security_id;
    metrics->active_rules = sec->firewall_rule_count;
    metrics->trusted_certificates = sec->certificate_count;
    metrics->total_packets_inspected = sec->total_packets_inspected;
    metrics->total_packets_blocked = sec->total_packets_blocked;
    metrics->total_threats_detected = sec->total_threats_detected;

    if (sec->total_packets_inspected > 0)
    {
        metrics->threat_level_percent = (float)(sec->total_threats_detected * 100.0f / sec->total_packets_inspected);
    }
    else
    {
        metrics->threat_level_percent = 0.0f;
    }

    return 0;
}
