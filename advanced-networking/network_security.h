#ifndef NETWORK_SECURITY_H
#define NETWORK_SECURITY_H

#include <stdint.h>
#include <stddef.h>

#define MAX_FIREWALL_RULES 1024
#define MAX_CERTIFICATES 256
#define MAX_CIPHER_SUITES 32
#define MAX_SECURITY_POLICIES 64
#define MAX_INTRUSION_ALERTS 10000

typedef enum
{
    FIREWALL_ACTION_ACCEPT,
    FIREWALL_ACTION_DROP,
    FIREWALL_ACTION_REJECT,
    FIREWALL_ACTION_LOG
} FirewallAction;

typedef enum
{
    RULE_DIRECTION_INBOUND,
    RULE_DIRECTION_OUTBOUND,
    RULE_DIRECTION_BIDIRECTIONAL
} RuleDirection;

typedef enum
{
    CIPHER_TLS_RSA,
    CIPHER_TLS_ECDHE,
    CIPHER_TLS_DHE,
    CIPHER_AES_128,
    CIPHER_AES_256,
    CIPHER_CHACHA20,
    CIPHER_3DES
} CipherSuite;

typedef struct
{
    uint32_t rule_id;
    uint32_t priority;

    char source_ip[64];
    char destination_ip[64];
    uint32_t source_port;
    uint32_t destination_port;
    char protocol[8]; // tcp, udp, icmp, etc

    RuleDirection direction;
    FirewallAction action;

    uint64_t packets_matched;
    uint64_t bytes_matched;

    uint32_t enabled;
} FirewallRule;

typedef struct
{
    uint32_t cert_id;
    char cert_name[64];

    uint8_t *cert_data;
    uint32_t cert_size;

    char issuer[128];
    char subject[128];

    uint32_t valid_from;
    uint32_t valid_until;

    uint32_t is_expired;
    uint32_t is_revoked;
} Certificate;

typedef struct
{
    uint32_t cipher_suite_id;
    CipherSuite cipher_type;
    uint32_t key_size; // 128, 256, etc

    uint32_t enabled;
    uint32_t supported;
} CipherSuiteConfig;

typedef struct
{
    uint32_t alert_id;
    uint32_t timestamp;

    char source_ip[64];
    char destination_ip[64];
    uint32_t source_port;
    uint32_t destination_port;

    char alert_type[64]; // DDoS, Port Scan, Brute Force, etc
    char description[256];

    uint32_t severity; // 1-10
    uint32_t action_taken;
} IntrusionAlert;

typedef struct
{
    uint32_t security_id;

    uint32_t firewall_rule_count;
    FirewallRule firewall_rules[MAX_FIREWALL_RULES];

    uint32_t certificate_count;
    Certificate certificates[MAX_CERTIFICATES];

    uint32_t cipher_suite_count;
    CipherSuiteConfig cipher_suites[MAX_CIPHER_SUITES];

    uint32_t security_policy_count;
    uint8_t security_policies[MAX_SECURITY_POLICIES];

    uint32_t intrusion_alert_count;
    IntrusionAlert intrusion_alerts[MAX_INTRUSION_ALERTS];

    uint64_t total_packets_inspected;
    uint64_t total_packets_blocked;
    uint64_t total_threats_detected;

    uint32_t ddos_protection_enabled;
    uint32_t intrusion_detection_enabled;
    uint32_t ssl_tls_enabled;
} NetworkSecurity;

// Security initialization
NetworkSecurity *network_security_init(void);
int network_security_destroy(NetworkSecurity *sec);

// Firewall management
int security_add_firewall_rule(NetworkSecurity *sec, const char *src_ip, const char *dst_ip,
                               uint32_t src_port, uint32_t dst_port, const char *protocol,
                               FirewallAction action);
int security_remove_firewall_rule(NetworkSecurity *sec, uint32_t rule_id);
int security_check_firewall_rule(NetworkSecurity *sec, const char *src_ip, const char *dst_ip,
                                 const char *protocol);
int security_enable_firewall(NetworkSecurity *sec);
int security_disable_firewall(NetworkSecurity *sec);

// Certificate management
int security_add_certificate(NetworkSecurity *sec, const char *cert_name, const uint8_t *cert_data, uint32_t cert_size);
int security_remove_certificate(NetworkSecurity *sec, uint32_t cert_id);
int security_verify_certificate(NetworkSecurity *sec, uint32_t cert_id);
int security_revoke_certificate(NetworkSecurity *sec, uint32_t cert_id);

// Cipher suite management
int security_add_cipher_suite(NetworkSecurity *sec, CipherSuite cipher, uint32_t key_size);
int security_remove_cipher_suite(NetworkSecurity *sec, uint32_t cipher_id);
int security_get_supported_ciphers(NetworkSecurity *sec);

// SSL/TLS management
int security_enable_ssl_tls(NetworkSecurity *sec);
int security_disable_ssl_tls(NetworkSecurity *sec);
int security_set_min_tls_version(NetworkSecurity *sec, uint32_t version);
int security_set_max_tls_version(NetworkSecurity *sec, uint32_t version);

// DDoS protection
int security_enable_ddos_protection(NetworkSecurity *sec);
int security_disable_ddos_protection(NetworkSecurity *sec);
int security_detect_ddos_attack(NetworkSecurity *sec);
int security_mitigate_ddos(NetworkSecurity *sec);

// Intrusion detection
int security_enable_intrusion_detection(NetworkSecurity *sec);
int security_disable_intrusion_detection(NetworkSecurity *sec);
int security_check_intrusion(NetworkSecurity *sec, const uint8_t *packet, uint32_t size);
int security_log_intrusion_alert(NetworkSecurity *sec, const char *alert_type, const char *description, uint32_t severity);

// Packet inspection
int security_inspect_packet(NetworkSecurity *sec, const uint8_t *packet, uint32_t size);
int security_block_packet(NetworkSecurity *sec);
int security_allow_packet(NetworkSecurity *sec);

// Monitoring
typedef struct
{
    uint32_t security_id;
    uint32_t active_rules;
    uint32_t trusted_certificates;
    uint64_t total_packets_inspected;
    uint64_t total_packets_blocked;
    uint64_t total_threats_detected;
    float threat_level_percent;
} NetworkSecurityMetrics;

int security_get_metrics(NetworkSecurity *sec, NetworkSecurityMetrics *metrics);

#endif 
