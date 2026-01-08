#ifndef CHECKPOINT_RECOVERY_H
#define CHECKPOINT_RECOVERY_H

#include <stdint.h>
#include <stddef.h>

#define MAX_CHECKPOINTS 256
#define MAX_RECOVERY_POINTS 512
#define MAX_PROCESS_STATE_SNAPSHOT 1048576

typedef enum
{
    CHECKPOINT_TYPE_FULL,
    CHECKPOINT_TYPE_INCREMENTAL,
    CHECKPOINT_TYPE_DIFFERENTIAL
} CheckpointType;

typedef enum
{
    CHECKPOINT_STATE_IDLE,
    CHECKPOINT_STATE_IN_PROGRESS,
    CHECKPOINT_STATE_COMPLETED,
    CHECKPOINT_STATE_FAILED
} CheckpointState;

typedef struct
{
    uint32_t checkpoint_id;
    CheckpointType type;
    CheckpointState state;

    uint64_t timestamp;
    uint32_t process_id;

    uint8_t *state_data;
    uint32_t state_size;
    uint32_t state_capacity;

    uint32_t page_count;
    uint32_t dirty_page_count;

    uint64_t compression_ratio;
    uint8_t is_compressed;
} Checkpoint;

typedef struct
{
    uint32_t recovery_point_id;
    uint32_t checkpoint_id;
    uint64_t timestamp;

    uint32_t process_id;
    uint64_t memory_state_hash;
    uint64_t register_state_hash;

    uint32_t recovery_success_count;
    uint32_t recovery_fail_count;
} RecoveryPoint;

typedef struct
{
    uint32_t manager_id;
    Checkpoint checkpoints[MAX_CHECKPOINTS];
    uint32_t checkpoint_count;

    RecoveryPoint recovery_points[MAX_RECOVERY_POINTS];
    uint32_t recovery_point_count;

    uint32_t last_successful_checkpoint;
    uint64_t last_checkpoint_time;

    uint64_t total_checkpoint_time_us;
    uint32_t total_checkpoints;
    uint32_t successful_recoveries;
    uint32_t failed_recoveries;
} CheckpointManager;

CheckpointManager *checkpoint_manager_init(void);
int checkpoint_manager_destroy(CheckpointManager *manager);

uint32_t checkpoint_create(CheckpointManager *manager, CheckpointType type, uint32_t process_id);
int checkpoint_save_state(CheckpointManager *manager, uint32_t checkpoint_id,
                          const uint8_t *state_data, uint32_t state_size);
int checkpoint_finalize(CheckpointManager *manager, uint32_t checkpoint_id);

int checkpoint_compress(CheckpointManager *manager, uint32_t checkpoint_id);
int checkpoint_decompress(CheckpointManager *manager, uint32_t checkpoint_id);

uint32_t recovery_create_point(CheckpointManager *manager, uint32_t checkpoint_id, uint32_t process_id);
int recovery_restore(CheckpointManager *manager, uint32_t recovery_point_id);

int recovery_rollback_to_checkpoint(CheckpointManager *manager, uint32_t checkpoint_id);
int recovery_list_checkpoints(CheckpointManager *manager, Checkpoint *checkpoints, uint32_t max_count);

int recovery_verify_checkpoint(CheckpointManager *manager, uint32_t checkpoint_id);
int recovery_prune_old_checkpoints(CheckpointManager *manager, uint32_t keep_count);

typedef struct
{
    uint32_t manager_id;
    uint32_t total_checkpoints;
    uint32_t successful_recoveries;
    uint32_t failed_recoveries;
    uint64_t average_checkpoint_time_us;
    float recovery_success_rate;
} CheckpointMetrics;

int checkpoint_get_metrics(CheckpointManager *manager, CheckpointMetrics *metrics);

#endif
