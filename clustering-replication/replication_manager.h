#ifndef REPLICATION_MANAGER_H
#define REPLICATION_MANAGER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_REPLICAS 32
#define MAX_REPLICATION_LOG 8192
#define MAX_REPLICATION_SESSIONS 256
#define MAX_REPLICA_DATA_SIZE 2097152

typedef enum
{
    REPLICATION_STRATEGY_SYNCHRONOUS,
    REPLICATION_STRATEGY_ASYNCHRONOUS,
    REPLICATION_STRATEGY_SEMI_SYNCHRONOUS
} ReplicationStrategy;

typedef enum
{
    CONSISTENCY_STRONG,
    CONSISTENCY_EVENTUAL,
    CONSISTENCY_CAUSAL
} ConsistencyModel;

typedef enum
{
    REPLICA_STATE_SYNCED,
    REPLICA_STATE_SYNCING,
    REPLICA_STATE_OUT_OF_SYNC,
    REPLICA_STATE_FAILED
} ReplicaState;

typedef struct
{
    uint32_t replica_id;
    uint32_t node_id;
    ReplicaState state;

    uint64_t last_sync_time;
    uint32_t sync_lag_ms;
    uint64_t bytes_replicated;

    uint32_t successful_syncs;
    uint32_t failed_syncs;
    float sync_success_rate;
} ReplicaInfo;

typedef struct
{
    uint32_t log_entry_id;
    uint32_t data_size;
    uint8_t data[256];

    uint64_t timestamp;
    uint32_t source_node_id;
    uint32_t replication_count;
} ReplicationLogEntry;

typedef struct
{
    uint32_t session_id;
    uint32_t source_node_id;
    ReplicaInfo replicas[MAX_REPLICAS];
    uint32_t replica_count;

    ReplicationLogEntry log[MAX_REPLICATION_LOG];
    uint32_t log_size;
    uint32_t log_head;
    uint32_t log_tail;

    ReplicationStrategy strategy;
    ConsistencyModel consistency;

    uint64_t total_bytes_replicated;
    uint32_t total_replication_events;
} ReplicationSession;

typedef struct
{
    uint32_t manager_id;
    ReplicationSession sessions[MAX_REPLICATION_SESSIONS];
    uint32_t session_count;

    uint64_t total_replicated_data;
    uint32_t total_replication_operations;
    uint32_t replication_failures;
    float replication_efficiency;
} ReplicationManager;

ReplicationManager *replication_manager_init(void);
int replication_manager_destroy(ReplicationManager *manager);

uint32_t replication_create_session(ReplicationManager *manager, uint32_t source_node_id,
                                    ReplicationStrategy strategy, ConsistencyModel consistency);
int replication_destroy_session(ReplicationManager *manager, uint32_t session_id);

int replication_add_replica(ReplicationManager *manager, uint32_t session_id,
                            uint32_t replica_id, uint32_t node_id);
int replication_remove_replica(ReplicationManager *manager, uint32_t session_id, uint32_t replica_id);

int replication_sync_replica(ReplicationManager *manager, uint32_t session_id, uint32_t replica_id);
int replication_detect_out_of_sync(ReplicationManager *manager, uint32_t session_id, uint32_t replica_id);

int replication_log_entry_add(ReplicationManager *manager, uint32_t session_id,
                              const uint8_t *data, uint32_t data_size, uint32_t source_node_id);
int replication_apply_log_entry(ReplicationManager *manager, uint32_t session_id, uint32_t log_id);

int replication_start_sync(ReplicationManager *manager, uint32_t session_id);
int replication_complete_sync(ReplicationManager *manager, uint32_t session_id);

int replication_check_consistency(ReplicationManager *manager, uint32_t session_id);
int replication_resolve_conflict(ReplicationManager *manager, uint32_t session_id);

int replication_update_replica_state(ReplicationManager *manager, uint32_t session_id,
                                     uint32_t replica_id, ReplicaState state);

typedef struct
{
    uint32_t manager_id;
    uint32_t active_sessions;
    uint32_t total_replicas;
    uint32_t synced_replicas;
    uint64_t total_replicated_bytes;
    uint32_t replication_operations;
    float replication_success_rate;
} ReplicationMetrics;

int replication_get_metrics(ReplicationManager *manager, ReplicationMetrics *metrics);

#endif
