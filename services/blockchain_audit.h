#ifndef BLOCKCHAIN_AUDIT_H
#define BLOCKCHAIN_AUDIT_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>

#define MAX_AUDIT_BLOCKS 4096
#define MAX_TRANSACTIONS_PER_BLOCK 256
#define MAX_AUDIT_NODES 32
#define MAX_HASH_SIZE 64
#define MAX_SIGNATURE_SIZE 256
#define MAX_EVENT_DATA 512
#define MAX_AUDIT_RULES 128

typedef enum
{
    EVENT_USER_LOGIN = 1,
    EVENT_FILE_ACCESS = 2,
    EVENT_FILE_MODIFY = 3,
    EVENT_FILE_DELETE = 4,
    EVENT_PERMISSION_CHANGE = 5,
    EVENT_SYSTEM_CONFIG = 6,
    EVENT_SECURITY_ALERT = 7,
    EVENT_RESOURCE_ALLOCATION = 8,
    EVENT_TRANSACTION = 9,
    EVENT_DEPLOYMENT = 10,
    EVENT_BACKUP = 11
} AuditEventType;

typedef enum
{
    SEVERITY_INFO = 1,
    SEVERITY_WARNING = 2,
    SEVERITY_CRITICAL = 3,
    SEVERITY_COMPLIANCE = 4
} EventSeverity;

typedef struct
{
    uint64_t transaction_id;
    uint64_t timestamp;
    uint32_t user_id;
    AuditEventType event_type;
    EventSeverity severity;
    char event_data[MAX_EVENT_DATA];
    char user_action[128];
    char resource[256];
    char result[64];
    char ip_address[64];
} AuditTransaction;

typedef struct
{
    uint32_t block_id;
    uint64_t timestamp;
    char previous_hash[MAX_HASH_SIZE];
    char merkle_root[MAX_HASH_SIZE];
    char block_hash[MAX_HASH_SIZE];
    char block_signature[MAX_SIGNATURE_SIZE];

    AuditTransaction transactions[MAX_TRANSACTIONS_PER_BLOCK];
    uint32_t transaction_count;
    uint32_t nonce;

    uint32_t miner_id;
    uint64_t difficulty;
} AuditBlock;

typedef struct
{
    uint32_t rule_id;
    AuditEventType event_type;
    EventSeverity min_severity;
    uint32_t is_enabled;
    uint32_t alert_on_violation;
} ComplianceRule;

typedef struct
{
    uint32_t audit_id;
    char name[64];

    AuditBlock blocks[MAX_AUDIT_BLOCKS];
    uint32_t block_count;

    ComplianceRule rules[MAX_AUDIT_RULES];
    uint32_t rule_count;

    uint32_t mining_enabled;
    uint32_t verification_enabled;

    uint64_t total_events;
    uint64_t total_transactions;
    uint64_t compliance_violations;
    uint64_t bytes_stored;
} BlockchainAudit;

int audit_create_blockchain(const char *name);
int audit_delete_blockchain(uint32_t audit_id);

int audit_add_event(uint32_t audit_id, uint32_t user_id, AuditEventType event_type,
                    EventSeverity severity, const char *event_data, const char *resource,
                    const char *ip_address);
int audit_add_transaction(uint32_t audit_id, AuditTransaction *transaction);

int audit_mine_block(uint32_t audit_id);
int audit_verify_block(uint32_t audit_id, uint32_t block_id);
int audit_verify_chain_integrity(uint32_t audit_id);

int audit_add_compliance_rule(uint32_t audit_id, AuditEventType event_type, EventSeverity severity);
int audit_remove_compliance_rule(uint32_t audit_id, uint32_t rule_id);
int audit_check_compliance(uint32_t audit_id);

int audit_query_events(uint32_t audit_id, AuditEventType event_type,
                       AuditTransaction *events, uint32_t *count);
int audit_query_by_user(uint32_t audit_id, uint32_t user_id,
                        AuditTransaction *events, uint32_t *count);
int audit_query_by_resource(uint32_id audit_id, const char *resource,
                            AuditTransaction *events, uint32_t *count);

int audit_get_block(uint32_t audit_id, uint32_t block_id, AuditBlock *block);
int audit_get_block_by_hash(uint32_t audit_id, const char *block_hash, AuditBlock *block);
int audit_get_chain_history(uint32_t audit_id, AuditBlock *blocks, uint32_t *count);

int audit_export_chain(uint32_t audit_id, const char *format);
int audit_generate_compliance_report(uint32_t audit_id, const char *output_path);
int audit_detect_tampering(uint32_t audit_id, uint32_t *tampered_block_ids, uint32_t *count);

uint64_t audit_get_event_count(uint32_t audit_id);
uint64_t audit_get_compliance_violations(uint32_t audit_id);
uint64_t audit_get_chain_size(uint32_t audit_id);

BlockchainAudit *audit_get_blockchain_info(uint32_t audit_id);

#endif
