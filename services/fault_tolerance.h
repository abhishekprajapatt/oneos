#ifndef FAULT_TOLERANCE_H
#define FAULT_TOLERANCE_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>

#define MAX_RECOVERY_POINTS 256
#define MAX_REPLICAS 5
#define MAX_CHECKPOINTS 128
#define MAX_RECOVERY_JOURNAL 4096

typedef enum
{
    REPLICA_ACTIVE = 0,
    REPLICA_STANDBY = 1,
    REPLICA_SYNCING = 2,
    REPLICA_FAILED = 3
} ReplicaState;

typedef enum
{
    CHECKPOINT_PENDING = 0,
    CHECKPOINT_COMPLETED = 1,
    CHECKPOINT_FAILED = 2
} CheckpointState;

typedef enum
{
    RECOVERY_SUCCESS = 0,
    RECOVERY_PARTIAL = 1,
    RECOVERY_FAILED = 2
} RecoveryResult;

typedef struct
{
    uint64_t checkpoint_id;
    uint64_t timestamp;
    uint32_t source_node_id;
    uint64_t data_size;
    uint32_t crc32;
    CheckpointState state;
    uint64_t recovery_point_id;
} Checkpoint;

typedef struct
{
    uint64_t recovery_point_id;
    uint64_t created_time;
    uint32_t checkpoint_count;
    Checkpoint checkpoints[MAX_CHECKPOINTS];
    uint8_t is_complete;
    uint8_t is_consistent;
    char description[256];
} RecoveryPoint;

typedef struct
{
    uint32_t replica_id;
    uint32_t node_id;
    uint32_t ip_address;
    uint16_t port;
    ReplicaState state;
    uint64_t last_sync_time;
    uint64_t bytes_synced;
    uint32_t lag_bytes;
    uint64_t sync_failures;
    uint64_t total_syncs;
} ReplicaInfo;

typedef struct
{
    uint32_t source_node_id;
    uint32_t target_node_id;
    uint64_t start_time;
    uint64_t completion_time;
    uint32_t duration_ms;
    uint64_t bytes_transferred;
    uint8_t success;
    char failure_reason[256];
} FailoverRecord;

typedef struct
{
    uint32_t system_id;
    char name[64];
    uint32_t primary_node_id;
    uint32_t replica_count;
    ReplicaInfo replicas[MAX_REPLICAS];
    uint32_t recovery_point_count;
    RecoveryPoint recovery_points[MAX_RECOVERY_POINTS];
    FailoverRecord last_failover;
    uint64_t total_failovers;
    uint64_t successful_recoveries;
    uint64_t failed_recoveries;
    uint32_t checkpoint_interval_ms;
    uint8_t auto_failover_enabled;
} FaultToleranceSystem;

// System initialization
int ft_create_system(const char *name, uint32_t primary_node_id);
int ft_add_replica(uint32_t ft_id, uint32_t node_id, uint32_t ip, uint16_t port);
int ft_remove_replica(uint32_t ft_id, uint32_t replica_id);

// Checkpointing
int ft_create_checkpoint(uint32_t ft_id, const void *data, size_t data_size);
int ft_mark_checkpoint_complete(uint32_t ft_id, uint64_t checkpoint_id);
Checkpoint *ft_get_checkpoint(uint32_t ft_id, uint64_t checkpoint_id);

// Recovery points
int ft_create_recovery_point(uint32_t ft_id, const char *description);
int ft_validate_recovery_point(uint32_t ft_id, uint64_t recovery_point_id);
RecoveryPoint *ft_get_recovery_point(uint32_t ft_id, uint64_t recovery_point_id);
int ft_restore_from_recovery_point(uint32_t ft_id, uint64_t recovery_point_id);

// Replication management
int ft_sync_replica(uint32_t ft_id, uint32_t replica_id, const void *data, size_t data_size);
int ft_set_replica_state(uint32_t ft_id, uint32_t replica_id, ReplicaState state);
ReplicaState ft_get_replica_state(uint32_t ft_id, uint32_t replica_id);

// Failover and recovery
int ft_check_primary_health(uint32_t ft_id);
int ft_initiate_failover(uint32_t ft_id, uint32_t new_primary_id);
int ft_perform_automatic_failover(uint32_t ft_id);
int ft_recovery_mode(uint32_t ft_id);

// Consistency and integrity
int ft_verify_replica_consistency(uint32_t ft_id);
int ft_resync_failed_replica(uint32_t ft_id, uint32_t replica_id);
int ft_heal_split_brain(uint32_t ft_id);

// Configuration
int ft_set_checkpoint_interval(uint32_t ft_id, uint32_t interval_ms);
int ft_enable_auto_failover(uint32_t ft_id);
int ft_disable_auto_failover(uint32_t ft_id);

// Statistics and monitoring
FaultToleranceSystem *ft_get_system_info(uint32_t ft_id);
uint64_t ft_get_recovery_success_rate(uint32_t ft_id);
uint32_t ft_get_replica_lag(uint32_t ft_id, uint32_t replica_id);
int ft_cleanup_old_checkpoints(uint32_t ft_id, uint64_t keep_recent);

#endif // FAULT_TOLERANCE_H
