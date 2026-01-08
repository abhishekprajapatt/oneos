#include "checkpoint_recovery.h"
#include <stdlib.h>
#include <string.h>

static CheckpointManager *global_checkpoint_manager = NULL;
static uint32_t next_checkpoint_id = 1;
static uint32_t next_recovery_point_id = 1;

CheckpointManager *checkpoint_manager_init(void)
{
    CheckpointManager *manager = (CheckpointManager *)malloc(sizeof(CheckpointManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(CheckpointManager));
    manager->manager_id = 1;
    manager->checkpoint_count = 0;
    manager->recovery_point_count = 0;
    manager->last_successful_checkpoint = 0;
    manager->last_checkpoint_time = 0;
    manager->total_checkpoint_time_us = 0;
    manager->total_checkpoints = 0;
    manager->successful_recoveries = 0;
    manager->failed_recoveries = 0;

    global_checkpoint_manager = manager;
    return manager;
}

int checkpoint_manager_destroy(CheckpointManager *manager)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->checkpoint_count; i++)
    {
        if (manager->checkpoints[i].state_data)
            free(manager->checkpoints[i].state_data);
    }

    free(manager);
    global_checkpoint_manager = NULL;
    return 0;
}

uint32_t checkpoint_create(CheckpointManager *manager, CheckpointType type, uint32_t process_id)
{
    if (!manager || manager->checkpoint_count >= MAX_CHECKPOINTS)
        return 0;

    Checkpoint *checkpoint = &manager->checkpoints[manager->checkpoint_count];
    memset(checkpoint, 0, sizeof(Checkpoint));

    checkpoint->checkpoint_id = next_checkpoint_id++;
    checkpoint->type = type;
    checkpoint->state = CHECKPOINT_STATE_IN_PROGRESS;
    checkpoint->timestamp = 0;
    checkpoint->process_id = process_id;
    checkpoint->state_capacity = MAX_PROCESS_STATE_SNAPSHOT;
    checkpoint->state_data = (uint8_t *)malloc(checkpoint->state_capacity);

    if (!checkpoint->state_data)
        return 0;

    checkpoint->state_size = 0;
    checkpoint->page_count = 0;
    checkpoint->dirty_page_count = 0;
    checkpoint->compression_ratio = 0;
    checkpoint->is_compressed = 0;

    manager->checkpoint_count++;
    manager->total_checkpoints++;

    return checkpoint->checkpoint_id;
}

int checkpoint_save_state(CheckpointManager *manager, uint32_t checkpoint_id,
                          const uint8_t *state_data, uint32_t state_size)
{
    if (!manager || checkpoint_id == 0 || !state_data || state_size == 0)
        return -1;

    for (uint32_t i = 0; i < manager->checkpoint_count; i++)
    {
        if (manager->checkpoints[i].checkpoint_id == checkpoint_id)
        {
            Checkpoint *cp = &manager->checkpoints[i];

            if (state_size > cp->state_capacity)
                return -1;

            memcpy(cp->state_data, state_data, state_size);
            cp->state_size = state_size;
            cp->page_count = state_size / 4096;

            return 0;
        }
    }

    return -1;
}

int checkpoint_finalize(CheckpointManager *manager, uint32_t checkpoint_id)
{
    if (!manager || checkpoint_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->checkpoint_count; i++)
    {
        if (manager->checkpoints[i].checkpoint_id == checkpoint_id)
        {
            Checkpoint *cp = &manager->checkpoints[i];
            cp->state = CHECKPOINT_STATE_COMPLETED;
            manager->last_successful_checkpoint = checkpoint_id;
            manager->last_checkpoint_time = 0;
            manager->total_checkpoint_time_us += 1000000;
            return 0;
        }
    }

    return -1;
}

int checkpoint_compress(CheckpointManager *manager, uint32_t checkpoint_id)
{
    if (!manager || checkpoint_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->checkpoint_count; i++)
    {
        if (manager->checkpoints[i].checkpoint_id == checkpoint_id)
        {
            Checkpoint *cp = &manager->checkpoints[i];
            cp->is_compressed = 1;
            cp->compression_ratio = 65;
            return 0;
        }
    }

    return -1;
}

int checkpoint_decompress(CheckpointManager *manager, uint32_t checkpoint_id)
{
    if (!manager || checkpoint_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->checkpoint_count; i++)
    {
        if (manager->checkpoints[i].checkpoint_id == checkpoint_id)
        {
            Checkpoint *cp = &manager->checkpoints[i];
            cp->is_compressed = 0;
            return 0;
        }
    }

    return -1;
}

uint32_t recovery_create_point(CheckpointManager *manager, uint32_t checkpoint_id, uint32_t process_id)
{
    if (!manager || checkpoint_id == 0 || manager->recovery_point_count >= MAX_RECOVERY_POINTS)
        return 0;

    RecoveryPoint *rp = &manager->recovery_points[manager->recovery_point_count];
    memset(rp, 0, sizeof(RecoveryPoint));

    rp->recovery_point_id = next_recovery_point_id++;
    rp->checkpoint_id = checkpoint_id;
    rp->timestamp = 0;
    rp->process_id = process_id;
    rp->memory_state_hash = (uint64_t)checkpoint_id * 31;
    rp->register_state_hash = (uint64_t)process_id * 37;
    rp->recovery_success_count = 0;
    rp->recovery_fail_count = 0;

    manager->recovery_point_count++;
    return rp->recovery_point_id;
}

int recovery_restore(CheckpointManager *manager, uint32_t recovery_point_id)
{
    if (!manager || recovery_point_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->recovery_point_count; i++)
    {
        if (manager->recovery_points[i].recovery_point_id == recovery_point_id)
        {
            RecoveryPoint *rp = &manager->recovery_points[i];
            rp->recovery_success_count++;
            manager->successful_recoveries++;
            return 0;
        }
    }

    manager->failed_recoveries++;
    return -1;
}

int recovery_rollback_to_checkpoint(CheckpointManager *manager, uint32_t checkpoint_id)
{
    if (!manager || checkpoint_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->checkpoint_count; i++)
    {
        if (manager->checkpoints[i].checkpoint_id == checkpoint_id)
        {
            if (manager->checkpoints[i].state == CHECKPOINT_STATE_COMPLETED)
            {
                manager->successful_recoveries++;
                return 0;
            }
        }
    }

    manager->failed_recoveries++;
    return -1;
}

int recovery_list_checkpoints(CheckpointManager *manager, Checkpoint *checkpoints, uint32_t max_count)
{
    if (!manager || !checkpoints || max_count == 0)
        return -1;

    uint32_t to_copy = manager->checkpoint_count < max_count ? manager->checkpoint_count : max_count;
    memcpy(checkpoints, manager->checkpoints, sizeof(Checkpoint) * to_copy);

    return to_copy;
}

int recovery_verify_checkpoint(CheckpointManager *manager, uint32_t checkpoint_id)
{
    if (!manager || checkpoint_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->checkpoint_count; i++)
    {
        if (manager->checkpoints[i].checkpoint_id == checkpoint_id)
        {
            Checkpoint *cp = &manager->checkpoints[i];
            if (cp->state == CHECKPOINT_STATE_COMPLETED && cp->state_size > 0)
            {
                return 0;
            }
        }
    }

    return -1;
}

int recovery_prune_old_checkpoints(CheckpointManager *manager, uint32_t keep_count)
{
    if (!manager || keep_count == 0)
        return -1;

    if (manager->checkpoint_count > keep_count)
    {
        uint32_t to_remove = manager->checkpoint_count - keep_count;

        for (uint32_t i = 0; i < to_remove; i++)
        {
            if (manager->checkpoints[i].state_data)
                free(manager->checkpoints[i].state_data);
        }

        memmove(manager->checkpoints, &manager->checkpoints[to_remove],
                sizeof(Checkpoint) * keep_count);
        manager->checkpoint_count = keep_count;

        return to_remove;
    }

    return 0;
}

int checkpoint_get_metrics(CheckpointManager *manager, CheckpointMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    metrics->manager_id = manager->manager_id;
    metrics->total_checkpoints = manager->total_checkpoints;
    metrics->successful_recoveries = manager->successful_recoveries;
    metrics->failed_recoveries = manager->failed_recoveries;

    if (manager->total_checkpoints > 0)
    {
        metrics->average_checkpoint_time_us = manager->total_checkpoint_time_us / manager->total_checkpoints;
    }
    else
    {
        metrics->average_checkpoint_time_us = 0;
    }

    if (manager->successful_recoveries + manager->failed_recoveries > 0)
    {
        metrics->recovery_success_rate = (float)manager->successful_recoveries /
                                         (manager->successful_recoveries + manager->failed_recoveries);
    }
    else
    {
        metrics->recovery_success_rate = 0.0f;
    }

    return 0;
}
