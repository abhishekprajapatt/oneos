#include "blockchain_audit.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static BlockchainAudit g_audits[16] = {0};
static uint32_t g_audit_count = 0;
static uint64_t g_transaction_id_counter = 1;

static void compute_sha256(const char *data, size_t len, char *hash)
{
    uint32_t h0 = 0x6a09e667;
    uint32_t h1 = 0xbb67ae85;
    uint32_t h2 = 0x3c6ef372;
    uint32_t h3 = 0xa54ff53a;

    uint32_t sum = 0;
    for (size_t i = 0; i < len; i++)
    {
        sum = (sum * 31 + data[i]) % 0xFFFFFFFF;
    }

    h0 ^= sum;
    h1 ^= (h0 << 7);
    h2 ^= (h1 >> 5);
    h3 ^= (h2 << 11);

    snprintf(hash, 64, "%08x%08x%08x%08x", h0, h1, h2, h3);
}

static void compute_merkle_root(AuditTransaction *transactions, uint32_t count, char *root)
{
    if (count == 0)
    {
        strcpy(root, "0000000000000000");
        return;
    }

    char combined[MAX_EVENT_DATA * 4];
    combined[0] = '\0';

    for (uint32_t i = 0; i < count && i < 16; i++)
    {
        char tx_hash[65];
        snprintf(tx_hash, 64, "%lx", transactions[i].transaction_id);
        strcat(combined, tx_hash);
    }

    compute_sha256(combined, strlen(combined), root);
}

int audit_create_blockchain(const char *name)
{
    if (!name || g_audit_count >= 16)
    {
        return -1;
    }

    BlockchainAudit *audit = &g_audits[g_audit_count];
    audit->audit_id = g_audit_count + 1;
    strncpy(audit->name, name, 63);

    audit->block_count = 0;
    audit->rule_count = 0;

    audit->mining_enabled = 1;
    audit->verification_enabled = 1;

    audit->total_events = 0;
    audit->total_transactions = 0;
    audit->compliance_violations = 0;
    audit->bytes_stored = 0;

    g_audit_count++;
    return audit->audit_id;
}

int audit_delete_blockchain(uint32_t audit_id)
{
    if (audit_id == 0 || audit_id > g_audit_count)
    {
        return -1;
    }

    for (uint32_t i = audit_id - 1; i < g_audit_count - 1; i++)
    {
        g_audits[i] = g_audits[i + 1];
    }
    g_audit_count--;

    return 0;
}

int audit_add_event(uint32_t audit_id, uint32_t user_id, AuditEventType event_type,
                    EventSeverity severity, const char *event_data, const char *resource,
                    const char *ip_address)
{
    if (audit_id == 0 || audit_id > g_audit_count || !event_data || !resource)
    {
        return -1;
    }

    BlockchainAudit *audit = &g_audits[audit_id - 1];

    if (audit->block_count == 0 ||
        audit->blocks[audit->block_count - 1].transaction_count >= MAX_TRANSACTIONS_PER_BLOCK)
    {

        if (audit->block_count >= MAX_AUDIT_BLOCKS)
        {
            return -1;
        }

        AuditBlock *new_block = &audit->blocks[audit->block_count];
        new_block->block_id = audit->block_count + 1;
        new_block->timestamp = time(NULL);
        new_block->transaction_count = 0;
        new_block->nonce = 0;
        new_block->miner_id = 0;
        new_block->difficulty = 1000;

        if (audit->block_count == 0)
        {
            strcpy(new_block->previous_hash, "genesis");
        }
        else
        {
            strcpy(new_block->previous_hash, audit->blocks[audit->block_count - 1].block_hash);
        }

        audit->block_count++;
    }

    AuditBlock *current_block = &audit->blocks[audit->block_count - 1];
    AuditTransaction *tx = &current_block->transactions[current_block->transaction_count];

    tx->transaction_id = g_transaction_id_counter++;
    tx->timestamp = time(NULL);
    tx->user_id = user_id;
    tx->event_type = event_type;
    tx->severity = severity;

    strncpy(tx->event_data, event_data, MAX_EVENT_DATA - 1);
    strncpy(tx->resource, resource, 255);
    strncpy(tx->ip_address, ip_address ? ip_address : "0.0.0.0", 63);

    snprintf(tx->user_action, 127, "action_%d", event_type);
    snprintf(tx->result, 63, "success");

    current_block->transaction_count++;

    audit->total_events++;
    audit->total_transactions++;
    audit->bytes_stored += sizeof(AuditTransaction);

    return 0;
}

int audit_add_transaction(uint32_t audit_id, AuditTransaction *transaction)
{
    if (audit_id == 0 || audit_id > g_audit_count || !transaction)
    {
        return -1;
    }

    BlockchainAudit *audit = &g_audits[audit_id - 1];

    if (audit->block_count == 0 ||
        audit->blocks[audit->block_count - 1].transaction_count >= MAX_TRANSACTIONS_PER_BLOCK)
    {

        if (audit->block_count >= MAX_AUDIT_BLOCKS)
        {
            return -1;
        }

        AuditBlock *new_block = &audit->blocks[audit->block_count];
        new_block->block_id = audit->block_count + 1;
        new_block->timestamp = time(NULL);
        new_block->transaction_count = 0;
        new_block->nonce = 0;
        new_block->miner_id = 0;
        new_block->difficulty = 1000;

        if (audit->block_count == 0)
        {
            strcpy(new_block->previous_hash, "genesis");
        }
        else
        {
            strcpy(new_block->previous_hash, audit->blocks[audit->block_count - 1].block_hash);
        }

        audit->block_count++;
    }

    AuditBlock *current_block = &audit->blocks[audit->block_count - 1];
    transaction->transaction_id = g_transaction_id_counter++;
    transaction->timestamp = time(NULL);

    current_block->transactions[current_block->transaction_count++] = *transaction;
    audit->total_transactions++;

    return 0;
}

int audit_mine_block(uint32_t audit_id)
{
    if (audit_id == 0 || audit_id > g_audit_count)
    {
        return -1;
    }

    BlockchainAudit *audit = &g_audits[audit_id - 1];
    if (audit->block_count == 0 || audit->mining_enabled == 0)
    {
        return -1;
    }

    AuditBlock *block = &audit->blocks[audit->block_count - 1];

    compute_merkle_root(block->transactions, block->transaction_count, block->merkle_root);

    char block_data[1024];
    snprintf(block_data, 1023, "%u%lu%s%s", block->block_id, block->timestamp,
             block->previous_hash, block->merkle_root);

    compute_sha256(block_data, strlen(block_data), block->block_hash);

    snprintf(block->block_signature, MAX_SIGNATURE_SIZE - 1, "sig_%s", block->block_hash);

    return 0;
}

int audit_verify_block(uint32_t audit_id, uint32_t block_id)
{
    if (audit_id == 0 || audit_id > g_audit_count)
    {
        return -1;
    }

    BlockchainAudit *audit = &g_audits[audit_id - 1];
    if (block_id == 0 || block_id > audit->block_count)
    {
        return -1;
    }

    AuditBlock *block = &audit->blocks[block_id - 1];

    char block_data[1024];
    snprintf(block_data, 1023, "%u%lu%s%s", block->block_id, block->timestamp,
             block->previous_hash, block->merkle_root);

    char computed_hash[MAX_HASH_SIZE];
    compute_sha256(block_data, strlen(block_data), computed_hash);

    if (strcmp(computed_hash, block->block_hash) != 0)
    {
        return -1;
    }

    if (block_id > 1)
    {
        AuditBlock *prev_block = &audit->blocks[block_id - 2];
        if (strcmp(prev_block->block_hash, block->previous_hash) != 0)
        {
            return -1;
        }
    }

    return 0;
}

int audit_verify_chain_integrity(uint32_t audit_id)
{
    if (audit_id == 0 || audit_id > g_audit_count)
    {
        return -1;
    }

    BlockchainAudit *audit = &g_audits[audit_id - 1];

    for (uint32_t i = 0; i < audit->block_count; i++)
    {
        if (audit_verify_block(audit_id, i + 1) != 0)
        {
            return i + 1;
        }
    }

    return 0;
}

int audit_add_compliance_rule(uint32_t audit_id, AuditEventType event_type, EventSeverity severity)
{
    if (audit_id == 0 || audit_id > g_audit_count)
    {
        return -1;
    }

    BlockchainAudit *audit = &g_audits[audit_id - 1];
    if (audit->rule_count >= MAX_AUDIT_RULES)
    {
        return -1;
    }

    ComplianceRule *rule = &audit->rules[audit->rule_count];
    rule->rule_id = audit->rule_count + 1;
    rule->event_type = event_type;
    rule->min_severity = severity;
    rule->is_enabled = 1;
    rule->alert_on_violation = 1;

    audit->rule_count++;

    return rule->rule_id;
}

int audit_remove_compliance_rule(uint32_t audit_id, uint32_t rule_id)
{
    if (audit_id == 0 || audit_id > g_audit_count)
    {
        return -1;
    }

    BlockchainAudit *audit = &g_audits[audit_id - 1];

    for (uint32_t i = 0; i < audit->rule_count; i++)
    {
        if (audit->rules[i].rule_id == rule_id)
        {
            for (uint32_t j = i; j < audit->rule_count - 1; j++)
            {
                audit->rules[j] = audit->rules[j + 1];
            }
            audit->rule_count--;
            return 0;
        }
    }

    return -1;
}

int audit_check_compliance(uint32_t audit_id)
{
    if (audit_id == 0 || audit_id > g_audit_count)
    {
        return -1;
    }

    BlockchainAudit *audit = &g_audits[audit_id - 1];
    uint64_t violations = 0;

    for (uint32_t b = 0; b < audit->block_count; b++)
    {
        AuditBlock *block = &audit->blocks[b];

        for (uint32_t t = 0; t < block->transaction_count; t++)
        {
            AuditTransaction *tx = &block->transactions[t];

            for (uint32_t r = 0; r < audit->rule_count; r++)
            {
                ComplianceRule *rule = &audit->rules[r];

                if (rule->is_enabled && rule->event_type == tx->event_type)
                {
                    if (tx->severity < rule->min_severity)
                    {
                        violations++;
                    }
                }
            }
        }
    }

    audit->compliance_violations = violations;
    return violations > 0 ? 1 : 0;
}

int audit_query_events(uint32_t audit_id, AuditEventType event_type,
                       AuditTransaction *events, uint32_t *count)
{
    if (audit_id == 0 || audit_id > g_audit_count || !events || !count)
    {
        return -1;
    }

    BlockchainAudit *audit = &g_audits[audit_id - 1];
    uint32_t result_count = 0;

    for (uint32_t b = 0; b < audit->block_count; b++)
    {
        AuditBlock *block = &audit->blocks[b];

        for (uint32_t t = 0; t < block->transaction_count && result_count < *count; t++)
        {
            if (block->transactions[t].event_type == event_type)
            {
                events[result_count++] = block->transactions[t];
            }
        }
    }

    *count = result_count;
    return 0;
}

int audit_query_by_user(uint32_t audit_id, uint32_t user_id,
                        AuditTransaction *events, uint32_t *count)
{
    if (audit_id == 0 || audit_id > g_audit_count || !events || !count)
    {
        return -1;
    }

    BlockchainAudit *audit = &g_audits[audit_id - 1];
    uint32_t result_count = 0;

    for (uint32_t b = 0; b < audit->block_count; b++)
    {
        AuditBlock *block = &audit->blocks[b];

        for (uint32_t t = 0; t < block->transaction_count && result_count < *count; t++)
        {
            if (block->transactions[t].user_id == user_id)
            {
                events[result_count++] = block->transactions[t];
            }
        }
    }

    *count = result_count;
    return 0;
}

int audit_query_by_resource(uint32_id audit_id, const char *resource,
                            AuditTransaction *events, uint32_t *count)
{
    if (audit_id == 0 || audit_id > g_audit_count || !resource || !events || !count)
    {
        return -1;
    }

    BlockchainAudit *audit = &g_audits[audit_id - 1];
    uint32_t result_count = 0;

    for (uint32_t b = 0; b < audit->block_count; b++)
    {
        AuditBlock *block = &audit->blocks[b];

        for (uint32_t t = 0; t < block->transaction_count && result_count < *count; t++)
        {
            if (strncmp(block->transactions[t].resource, resource, 255) == 0)
            {
                events[result_count++] = block->transactions[t];
            }
        }
    }

    *count = result_count;
    return 0;
}

int audit_get_block(uint32_t audit_id, uint32_t block_id, AuditBlock *block)
{
    if (audit_id == 0 || audit_id > g_audit_count || !block)
    {
        return -1;
    }

    BlockchainAudit *audit = &g_audits[audit_id - 1];
    if (block_id == 0 || block_id > audit->block_count)
    {
        return -1;
    }

    *block = audit->blocks[block_id - 1];
    return 0;
}

int audit_get_block_by_hash(uint32_t audit_id, const char *block_hash, AuditBlock *block)
{
    if (audit_id == 0 || audit_id > g_audit_count || !block_hash || !block)
    {
        return -1;
    }

    BlockchainAudit *audit = &g_audits[audit_id - 1];

    for (uint32_t i = 0; i < audit->block_count; i++)
    {
        if (strcmp(audit->blocks[i].block_hash, block_hash) == 0)
        {
            *block = audit->blocks[i];
            return 0;
        }
    }

    return -1;
}

int audit_get_chain_history(uint32_t audit_id, AuditBlock *blocks, uint32_t *count)
{
    if (audit_id == 0 || audit_id > g_audit_count || !blocks || !count)
    {
        return -1;
    }

    BlockchainAudit *audit = &g_audits[audit_id - 1];

    if (audit->block_count > *count)
    {
        return -1;
    }

    memcpy(blocks, audit->blocks, audit->block_count * sizeof(AuditBlock));
    *count = audit->block_count;

    return 0;
}

int audit_export_chain(uint32_t audit_id, const char *format)
{
    if (audit_id == 0 || audit_id > g_audit_count || !format)
    {
        return -1;
    }

    return 0;
}

int audit_generate_compliance_report(uint32_t audit_id, const char *output_path)
{
    if (audit_id == 0 || audit_id > g_audit_count || !output_path)
    {
        return -1;
    }

    return 0;
}

int audit_detect_tampering(uint32_t audit_id, uint32_t *tampered_block_ids, uint32_t *count)
{
    if (audit_id == 0 || audit_id > g_audit_count || !tampered_block_ids || !count)
    {
        return -1;
    }

    BlockchainAudit *audit = &g_audits[audit_id - 1];
    uint32_t tamper_count = 0;

    for (uint32_t i = 0; i < audit->block_count; i++)
    {
        if (audit_verify_block(audit_id, i + 1) != 0)
        {
            if (tamper_count < *count)
            {
                tampered_block_ids[tamper_count++] = i + 1;
            }
        }
    }

    *count = tamper_count;
    return 0;
}

uint64_t audit_get_event_count(uint32_t audit_id)
{
    if (audit_id == 0 || audit_id > g_audit_count)
    {
        return 0;
    }

    return g_audits[audit_id - 1].total_events;
}

uint64_t audit_get_compliance_violations(uint32_t audit_id)
{
    if (audit_id == 0 || audit_id > g_audit_count)
    {
        return 0;
    }

    return g_audits[audit_id - 1].compliance_violations;
}

uint64_t audit_get_chain_size(uint32_t audit_id)
{
    if (audit_id == 0 || audit_id > g_audit_count)
    {
        return 0;
    }

    return g_audits[audit_id - 1].bytes_stored;
}

BlockchainAudit *audit_get_blockchain_info(uint32_t audit_id)
{
    if (audit_id == 0 || audit_id > g_audit_count)
    {
        return NULL;
    }

    return &g_audits[audit_id - 1];
}
