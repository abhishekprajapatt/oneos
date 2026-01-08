#ifndef DISTRIBUTED_STORAGE_H
#define DISTRIBUTED_STORAGE_H

#include <stdint.h>
#include <stddef.h>

#define MAX_REPLICAS 32
#define MAX_DATA_BLOCKS 100000
#define MAX_KEY_LEN 256
#define MAX_VALUE_LEN 1048576 // 1MB
#define MAX_REPLICATION_JOBS 128

typedef enum
{
    REPLICA_STATE_SYNCED,
    REPLICA_STATE_SYNCING,
    REPLICA_STATE_LAGGING,
    REPLICA_STATE_FAILED,
    REPLICA_STATE_RECOVERING
} ReplicaState;

typedef enum
{
    REPLICATION_STRATEGY_SYNCHRONOUS,
    REPLICATION_STRATEGY_ASYNCHRONOUS,
    REPLICATION_STRATEGY_QUORUM
} ReplicationStrategy;

typedef struct
{
    char key[MAX_KEY_LEN];
    uint8_t value[MAX_VALUE_LEN];
    uint64_t value_size;

    uint64_t version;
    uint32_t created_timestamp;
    uint32_t modified_timestamp;

    uint32_t replica_count;
    uint32_t replicas[MAX_REPLICAS];

    uint8_t is_tombstone; // Soft delete
} DataBlock;

typedef struct
{
    uint32_t replica_id;
    char replica_address[64];
    uint16_t port;

    ReplicaState state;
    uint64_t synced_offset;
    uint64_t total_data_size;

    uint32_t replication_lag_ms;
    uint32_t last_sync_time;

    uint64_t blocks_stored;
    uint64_t sync_success_count;
    uint64_t sync_fail_count;
} Replica;

typedef struct
{
    uint32_t replication_job_id;
    uint32_t source_replica_id;
    uint32_t target_replica_id;

    uint64_t blocks_to_replicate;
    uint64_t blocks_replicated;
    float progress_percent;

    uint32_t start_time;
    uint32_t end_time;
    uint8_t completed;
} ReplicationJob;

typedef struct
{
    uint32_t storage_node_id;
    char node_address[64];
    uint16_t port;

    uint32_t replica_count;
    Replica replicas[MAX_REPLICAS];

    uint64_t total_data_blocks;
    DataBlock *data_blocks;

    ReplicationStrategy replication_strategy;
    uint32_t replication_factor;
    uint32_t consistency_level;

    uint64_t total_read_ops;
    uint64_t total_write_ops;
    uint64_t total_delete_ops;

    uint64_t total_bytes_replicated;
    uint32_t replication_job_count;
    ReplicationJob replication_jobs[MAX_REPLICATION_JOBS];
} DistributedStorage;

// Storage initialization
DistributedStorage *distributed_storage_init(uint32_t node_id, ReplicationStrategy strategy);
int distributed_storage_destroy(DistributedStorage *ds);

// Replica management
int storage_add_replica(DistributedStorage *ds, const char *address, uint16_t port);
int storage_remove_replica(DistributedStorage *ds, uint32_t replica_id);
int storage_check_replica_health(Replica *replica);

// Data operations
int storage_put(DistributedStorage *ds, const char *key, const uint8_t *value, uint64_t value_size);
int storage_get(DistributedStorage *ds, const char *key, uint8_t *value, uint64_t *value_size);
int storage_delete(DistributedStorage *ds, const char *key);
int storage_exists(DistributedStorage *ds, const char *key);

// Replication operations
int storage_start_replication(DistributedStorage *ds, uint32_t source_replica_id, uint32_t target_replica_id);
int storage_sync_replica(DistributedStorage *ds, uint32_t replica_id);
int storage_handle_replica_failure(DistributedStorage *ds, uint32_t replica_id);
int storage_recover_replica(DistributedStorage *ds, uint32_t replica_id);

// Quorum operations
int storage_quorum_read(DistributedStorage *ds, const char *key, uint8_t *value, uint64_t *value_size);
int storage_quorum_write(DistributedStorage *ds, const char *key, const uint8_t *value, uint64_t value_size);

// Range queries and scanning
typedef struct
{
    char start_key[MAX_KEY_LEN];
    char end_key[MAX_KEY_LEN];
    uint32_t limit;
    uint64_t result_count;
    DataBlock *results;
} RangeQuery;

int storage_range_query(DistributedStorage *ds, RangeQuery *query);
int storage_scan_keys(DistributedStorage *ds, const char *pattern, DataBlock **results, uint64_t *result_count);

// TTL and expiration
int storage_set_ttl(DistributedStorage *ds, const char *key, uint32_t ttl_seconds);
int storage_expire_old_keys(DistributedStorage *ds);

// Compaction and cleanup
int storage_compact(DistributedStorage *ds);
int storage_defragment(DistributedStorage *ds);
int storage_cleanup_tombstones(DistributedStorage *ds);

// Monitoring
typedef struct
{
    uint32_t node_id;
    uint64_t total_keys;
    uint64_t total_replicas;
    uint64_t healthy_replicas;
    uint64_t total_data_size;
    float replication_factor_average;
    float write_latency_ms;
    float read_latency_ms;
} StorageMetrics;

int storage_get_metrics(DistributedStorage *ds, StorageMetrics *metrics);
int storage_get_replication_lag(DistributedStorage *ds);

#endif // DISTRIBUTED_STORAGE_H
