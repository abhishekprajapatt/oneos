#ifndef DIST_KV_STORE_H
#define DIST_KV_STORE_H

#include <stdint.h>
#include <stddef.h>

#define MAX_KV_STORES 16
#define MAX_NODES 64
#define MAX_KEYS 16384
#define MAX_KEY_SIZE 256
#define MAX_VALUE_SIZE 65536
#define MAX_REPLICAS 8
#define MAX_TRANSACTIONS 512
#define MAX_VERSIONS 32

typedef enum
{
    NODE_HEALTHY = 1,
    NODE_DEGRADED = 2,
    NODE_UNREACHABLE = 3,
    NODE_RECOVERING = 4
} NodeHealth;

typedef enum
{
    CONSISTENCY_STRONG = 1,
    CONSISTENCY_EVENTUAL = 2,
    CONSISTENCY_CAUSAL = 3
} ConsistencyModel;

typedef enum
{
    TX_PENDING = 1,
    TX_COMMITTED = 2,
    TX_ABORTED = 3,
    TX_FAILED = 4
} TransactionState;

typedef struct
{
    uint32_t version;
    uint64_t timestamp;
    char value[MAX_VALUE_SIZE];
    uint32_t value_len;
    char author[64];
} ValueVersion;

typedef struct
{
    char key[MAX_KEY_SIZE];
    ValueVersion versions[MAX_VERSIONS];
    uint32_t version_count;
    uint32_t current_version;
    uint64_t created_at;
    uint64_t last_modified;
    uint32_t access_count;
    uint32_t is_tombstone;
} KeyValueEntry;

typedef struct
{
    uint32_t node_id;
    char address[64];
    uint16_t port;
    NodeHealth health;
    uint64_t last_heartbeat;
    uint32_t replication_lag;
    uint64_t keys_stored;
    uint64_t bytes_stored;
} Node;

typedef struct
{
    uint64_t tx_id;
    char operation[32];
    char key[MAX_KEY_SIZE];
    char value[MAX_VALUE_SIZE];
    uint32_t value_len;
    TransactionState state;
    uint64_t start_time;
    uint64_t commit_time;
    uint32_t read_version;
    uint32_t write_version;
} Transaction;

typedef struct
{
    uint32_t store_id;
    char name[64];
    ConsistencyModel consistency;

    Node nodes[MAX_NODES];
    uint32_t node_count;

    KeyValueEntry *entries[MAX_KEYS];
    uint32_t entry_count;

    Transaction transactions[MAX_TRANSACTIONS];
    uint32_t tx_count;

    uint32_t replica_factor;
    uint32_t quorum_size;
    uint32_t hash_ring_size;
    uint32_t *hash_ring;

    uint64_t total_reads;
    uint64_t total_writes;
    uint64_t total_deletes;
    uint64_t bytes_total;
} DistributedKVStore;

typedef struct
{
    uint32_t node_id;
    uint64_t distance;
} ReplicaNode;

int kv_create_store(const char *name, ConsistencyModel consistency, uint32_t replica_factor);
int kv_delete_store(uint32_t store_id);
int kv_add_node(uint32_t store_id, uint32_t node_id, const char *address, uint16_t port);
int kv_remove_node(uint32_t store_id, uint32_t node_id);
int kv_get_node_health(uint32_t store_id, uint32_t node_id);
int kv_update_node_health(uint32_t store_id, uint32_t node_id, NodeHealth health);

int kv_put(uint32_t store_id, const char *key, const char *value, uint32_t value_len);
int kv_get(uint32_t store_id, const char *key, char *value, uint32_t *value_len);
int kv_delete(uint32_t store_id, const char *key);
int kv_exists(uint32_t store_id, const char *key);

int kv_get_version(uint32_t store_id, const char *key, uint32_t version, char *value, uint32_t *value_len);
int kv_get_all_versions(uint32_t store_id, const char *key, char **values, uint32_t *value_lens, uint32_t *version_count);
int kv_get_version_history(uint32_t store_id, const char *key, ValueVersion *versions, uint32_t *count);

uint64_t kv_start_transaction(uint32_t store_id, const char *operation, const char *key, const char *value, uint32_t value_len);
int kv_commit_transaction(uint32_t store_id, uint64_t tx_id);
int kv_abort_transaction(uint32_t store_id, uint64_t tx_id);
int kv_get_transaction_state(uint32_t store_id, uint64_t tx_id, TransactionState *state);

int kv_get_replicas(uint32_t store_id, const char *key, ReplicaNode *replicas, uint32_t *count);
int kv_replicate_key(uint32_t store_id, uint32_t from_node, uint32_t to_node, const char *key);
int kv_sync_node(uint32_t store_id, uint32_t node_id);

int kv_scan(uint32_t store_id, const char *key_prefix, KeyValueEntry *entries, uint32_t *count);
int kv_count_keys(uint32_t store_id, uint32_t *count);

int kv_enable_compression(uint32_t store_id, int enable);
int kv_compact_store(uint32_t store_id);
int kv_rebuild_hash_ring(uint32_t store_id);

uint64_t kv_get_total_reads(uint32_t store_id);
uint64_t kv_get_total_writes(uint32_t store_id);
uint64_t kv_get_total_deletes(uint32_t store_id);
uint64_t kv_get_total_bytes(uint32_t store_id);

DistributedKVStore *kv_get_store_info(uint32_t store_id);
int kv_export_store(uint32_t store_id, const char *format);

#endif
