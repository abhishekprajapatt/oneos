#include "fault_tolerance.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static FaultToleranceSystem g_ft_systems[8] = {0};
static uint32_t g_ft_count = 0;

uint32_t ft_crc32(const void *data, size_t size)
{
    uint32_t crc = 0xFFFFFFFF;
    const uint8_t *bytes = (const uint8_t *)data;

    for (size_t i = 0; i < size; i++)
    {
        crc ^= bytes[i];
        for (int j = 0; j < 8; j++)
        {
            if (crc & 1)
            {
                crc = (crc >> 1) ^ 0xEDB88320;
            }
            else
            {
                crc >>= 1;
            }
        }
    }

    return crc ^ 0xFFFFFFFF;
}

int ft_create_system(const char *name, uint32_t primary_node_id)
{
    if (!name || g_ft_count >= 8)
    {
        return -1;
    }

    FaultToleranceSystem *ft = &g_ft_systems[g_ft_count];
    ft->system_id = g_ft_count + 1;
    strncpy(ft->name, name, 63);
    ft->primary_node_id = primary_node_id;
    ft->replica_count = 0;
    ft->recovery_point_count = 0;
    ft->total_failovers = 0;
    ft->successful_recoveries = 0;
    ft->failed_recoveries = 0;
    ft->checkpoint_interval_ms = 5000;
    ft->auto_failover_enabled = 1;

    g_ft_count++;
    return ft->system_id;
}

int ft_add_replica(uint32_t ft_id, uint32_t node_id, uint32_t ip, uint16_t port)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];
    if (ft->replica_count >= MAX_REPLICAS)
    {
        return -1;
    }

    ReplicaInfo *replica = &ft->replicas[ft->replica_count];
    replica->replica_id = ft->replica_count + 1;
    replica->node_id = node_id;
    replica->ip_address = ip;
    replica->port = port;
    replica->state = REPLICA_STANDBY;
    replica->last_sync_time = 0;
    replica->bytes_synced = 0;
    replica->lag_bytes = 0;
    replica->sync_failures = 0;
    replica->total_syncs = 0;

    ft->replica_count++;
    return replica->replica_id;
}

int ft_remove_replica(uint32_t ft_id, uint32_t replica_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];
    if (replica_id == 0 || replica_id > ft->replica_count)
    {
        return -1;
    }

    for (uint32_t i = replica_id - 1; i < ft->replica_count - 1; i++)
    {
        ft->replicas[i] = ft->replicas[i + 1];
    }
    ft->replica_count--;

    return 0;
}

int ft_create_checkpoint(uint32_t ft_id, const void *data, size_t data_size)
{
    if (ft_id == 0 || ft_id > g_ft_count || !data)
    {
        return -1;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];

    if (ft->recovery_point_count >= MAX_RECOVERY_POINTS)
    {
        return -1;
    }

    RecoveryPoint *rp = &ft->recovery_points[ft->recovery_point_count];
    rp->recovery_point_id = ft->recovery_point_count + 1;
    rp->created_time = time(NULL);
    rp->checkpoint_count = 1;
    rp->is_complete = 0;
    rp->is_consistent = 1;

    Checkpoint *cp = &rp->checkpoints[0];
    cp->checkpoint_id = 1;
    cp->timestamp = time(NULL);
    cp->source_node_id = ft->primary_node_id;
    cp->data_size = data_size;
    cp->crc32 = ft_crc32(data, data_size);
    cp->state = CHECKPOINT_COMPLETED;
    cp->recovery_point_id = rp->recovery_point_id;

    ft->recovery_point_count++;

    return rp->recovery_point_id;
}

int ft_mark_checkpoint_complete(uint32_t ft_id, uint64_t checkpoint_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];

    for (uint32_t i = 0; i < ft->recovery_point_count; i++)
    {
        for (uint32_t j = 0; j < ft->recovery_points[i].checkpoint_count; j++)
        {
            if (ft->recovery_points[i].checkpoints[j].checkpoint_id == checkpoint_id)
            {
                ft->recovery_points[i].checkpoints[j].state = CHECKPOINT_COMPLETED;
                return 0;
            }
        }
    }

    return -1;
}

Checkpoint *ft_get_checkpoint(uint32_t ft_id, uint64_t checkpoint_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return NULL;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];

    for (uint32_t i = 0; i < ft->recovery_point_count; i++)
    {
        for (uint32_t j = 0; j < ft->recovery_points[i].checkpoint_count; j++)
        {
            if (ft->recovery_points[i].checkpoints[j].checkpoint_id == checkpoint_id)
            {
                return &ft->recovery_points[i].checkpoints[j];
            }
        }
    }

    return NULL;
}

int ft_create_recovery_point(uint32_t ft_id, const char *description)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];

    if (ft->recovery_point_count >= MAX_RECOVERY_POINTS)
    {
        return -1;
    }

    RecoveryPoint *rp = &ft->recovery_points[ft->recovery_point_count];
    rp->recovery_point_id = ft->recovery_point_count + 1;
    rp->created_time = time(NULL);
    rp->checkpoint_count = 0;
    rp->is_complete = 0;
    rp->is_consistent = 1;

    if (description)
    {
        strncpy(rp->description, description, 255);
    }

    ft->recovery_point_count++;

    return rp->recovery_point_id;
}

int ft_validate_recovery_point(uint32_t ft_id, uint64_t recovery_point_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];

    for (uint32_t i = 0; i < ft->recovery_point_count; i++)
    {
        if (ft->recovery_points[i].recovery_point_id == recovery_point_id)
        {
            if (ft->recovery_points[i].checkpoint_count > 0 &&
                ft->recovery_points[i].is_consistent)
            {
                return 1;
            }
            return 0;
        }
    }

    return -1;
}

RecoveryPoint *ft_get_recovery_point(uint32_t ft_id, uint64_t recovery_point_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return NULL;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];

    for (uint32_t i = 0; i < ft->recovery_point_count; i++)
    {
        if (ft->recovery_points[i].recovery_point_id == recovery_point_id)
        {
            return &ft->recovery_points[i];
        }
    }

    return NULL;
}

int ft_restore_from_recovery_point(uint32_t ft_id, uint64_t recovery_point_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];
    RecoveryPoint *rp = ft_get_recovery_point(ft_id, recovery_point_id);

    if (!rp || rp->checkpoint_count == 0)
    {
        ft->failed_recoveries++;
        return -1;
    }

    if (rp->is_consistent)
    {
        ft->successful_recoveries++;
        return 0;
    }

    ft->failed_recoveries++;
    return -1;
}

int ft_sync_replica(uint32_t ft_id, uint32_t replica_id, const void *data, size_t data_size)
{
    if (ft_id == 0 || ft_id > g_ft_count || !data)
    {
        return -1;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];
    if (replica_id == 0 || replica_id > ft->replica_count)
    {
        return -1;
    }

    ReplicaInfo *replica = &ft->replicas[replica_id - 1];
    replica->bytes_synced += data_size;
    replica->last_sync_time = time(NULL);
    replica->total_syncs++;
    replica->state = REPLICA_SYNCING;

    if (replica->state == REPLICA_ACTIVE)
    {
        replica->state = REPLICA_ACTIVE;
    }
    else
    {
        replica->state = REPLICA_STANDBY;
    }

    return 0;
}

int ft_set_replica_state(uint32_t ft_id, uint32_t replica_id, ReplicaState state)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];
    if (replica_id == 0 || replica_id > ft->replica_count)
    {
        return -1;
    }

    ft->replicas[replica_id - 1].state = state;
    return 0;
}

ReplicaState ft_get_replica_state(uint32_t ft_id, uint32_t replica_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return REPLICA_FAILED;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];
    if (replica_id == 0 || replica_id > ft->replica_count)
    {
        return REPLICA_FAILED;
    }

    return ft->replicas[replica_id - 1].state;
}

int ft_check_primary_health(uint32_t ft_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    return 1;
}

int ft_initiate_failover(uint32_t ft_id, uint32_t new_primary_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];
    ft->last_failover.start_time = time(NULL);
    ft->last_failover.source_node_id = ft->primary_node_id;
    ft->last_failover.target_node_id = new_primary_id;

    ft->primary_node_id = new_primary_id;
    ft->total_failovers++;
    ft->last_failover.success = 1;
    ft->last_failover.completion_time = time(NULL);
    ft->last_failover.duration_ms =
        (ft->last_failover.completion_time - ft->last_failover.start_time) * 1000;

    return 0;
}

int ft_perform_automatic_failover(uint32_t ft_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];

    if (!ft->auto_failover_enabled)
    {
        return -1;
    }

    for (uint32_t i = 0; i < ft->replica_count; i++)
    {
        if (ft->replicas[i].state == REPLICA_STANDBY)
        {
            return ft_initiate_failover(ft_id, ft->replicas[i].node_id);
        }
    }

    return -1;
}

int ft_recovery_mode(uint32_t ft_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    return 0;
}

int ft_verify_replica_consistency(uint32_t ft_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];

    for (uint32_t i = 0; i < ft->replica_count; i++)
    {
        if (ft->replicas[i].state != REPLICA_ACTIVE &&
            ft->replicas[i].state != REPLICA_STANDBY)
        {
            return 0;
        }
    }

    return 1;
}

int ft_resync_failed_replica(uint32_t ft_id, uint32_t replica_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];
    if (replica_id == 0 || replica_id > ft->replica_count)
    {
        return -1;
    }

    ReplicaInfo *replica = &ft->replicas[replica_id - 1];
    replica->state = REPLICA_SYNCING;
    replica->sync_failures = 0;
    replica->lag_bytes = 0;

    return 0;
}

int ft_heal_split_brain(uint32_t ft_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    return 0;
}

int ft_set_checkpoint_interval(uint32_t ft_id, uint32_t interval_ms)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    g_ft_systems[ft_id - 1].checkpoint_interval_ms = interval_ms;
    return 0;
}

int ft_enable_auto_failover(uint32_t ft_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    g_ft_systems[ft_id - 1].auto_failover_enabled = 1;
    return 0;
}

int ft_disable_auto_failover(uint32_t ft_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    g_ft_systems[ft_id - 1].auto_failover_enabled = 0;
    return 0;
}

FaultToleranceSystem *ft_get_system_info(uint32_t ft_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return NULL;
    }

    return &g_ft_systems[ft_id - 1];
}

uint64_t ft_get_recovery_success_rate(uint32_t ft_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return 0;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];
    uint64_t total = ft->successful_recoveries + ft->failed_recoveries;

    if (total == 0)
        return 0;
    return (ft->successful_recoveries * 100) / total;
}

uint32_t ft_get_replica_lag(uint32_t ft_id, uint32_t replica_id)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return 0;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];
    if (replica_id == 0 || replica_id > ft->replica_count)
    {
        return 0;
    }

    return ft->replicas[replica_id - 1].lag_bytes;
}

int ft_cleanup_old_checkpoints(uint32_t ft_id, uint64_t keep_recent)
{
    if (ft_id == 0 || ft_id > g_ft_count)
    {
        return -1;
    }

    FaultToleranceSystem *ft = &g_ft_systems[ft_id - 1];
    int removed = 0;

    while (ft->recovery_point_count > keep_recent)
    {
        for (uint32_t i = 0; i < ft->recovery_point_count - 1; i++)
        {
            ft->recovery_points[i] = ft->recovery_points[i + 1];
        }
        ft->recovery_point_count--;
        removed++;
    }

    return removed;
}
