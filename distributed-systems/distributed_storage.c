#include "distributed_storage.h"
#include <stdlib.h>
#include <string.h>

static DistributedStorage *global_storage = NULL;
static uint32_t next_replica_id = 1;
static uint32_t next_job_id = 1;

DistributedStorage *distributed_storage_init(uint32_t node_id, ReplicationStrategy strategy)
{
    DistributedStorage *ds = (DistributedStorage *)malloc(sizeof(DistributedStorage));
    if (!ds)
        return NULL;

    memset(ds, 0, sizeof(DistributedStorage));

    ds->storage_node_id = node_id;
    strncpy(ds->node_address, "127.0.0.1", 63);
    ds->port = 6000 + node_id;

    ds->replica_count = 0;
    ds->total_data_blocks = 0;
    ds->replication_strategy = strategy;
    ds->replication_factor = 3; // Default 3-way replication
    ds->consistency_level = 2;  // Quorum read

    ds->total_read_ops = 0;
    ds->total_write_ops = 0;
    ds->total_delete_ops = 0;
    ds->total_bytes_replicated = 0;
    ds->replication_job_count = 0;

    ds->data_blocks = (DataBlock *)malloc(sizeof(DataBlock) * MAX_DATA_BLOCKS);
    if (!ds->data_blocks)
    {
        free(ds);
        return NULL;
    }

    memset(ds->data_blocks, 0, sizeof(DataBlock) * MAX_DATA_BLOCKS);

    global_storage = ds;
    return ds;
}

int distributed_storage_destroy(DistributedStorage *ds)
{
    if (!ds)
        return -1;

    if (ds->data_blocks)
        free(ds->data_blocks);

    free(ds);
    global_storage = NULL;
    return 0;
}

int storage_add_replica(DistributedStorage *ds, const char *address, uint16_t port)
{
    if (!ds || ds->replica_count >= MAX_REPLICAS || !address)
        return -1;

    Replica *replica = &ds->replicas[ds->replica_count];
    memset(replica, 0, sizeof(Replica));

    replica->replica_id = next_replica_id++;
    strncpy(replica->replica_address, address, 63);
    replica->port = port;

    replica->state = REPLICA_STATE_SYNCING;
    replica->synced_offset = 0;
    replica->total_data_size = 0;
    replica->replication_lag_ms = 0;
    replica->last_sync_time = 0;
    replica->blocks_stored = 0;
    replica->sync_success_count = 0;
    replica->sync_fail_count = 0;

    ds->replica_count++;
    return 0;
}

int storage_remove_replica(DistributedStorage *ds, uint32_t replica_id)
{
    if (!ds)
        return -1;

    for (uint32_t i = 0; i < ds->replica_count; i++)
    {
        if (ds->replicas[i].replica_id == replica_id)
        {
            ds->replicas[i].state = REPLICA_STATE_FAILED;
            return 0;
        }
    }

    return -1;
}

int storage_check_replica_health(Replica *replica)
{
    if (!replica)
        return -1;

    if (replica->replication_lag_ms > 5000)
    {
        replica->state = REPLICA_STATE_LAGGING;
    }
    else if (replica->state == REPLICA_STATE_SYNCING && replica->replication_lag_ms == 0)
    {
        replica->state = REPLICA_STATE_SYNCED;
    }

    return 0;
}

int storage_put(DistributedStorage *ds, const char *key, const uint8_t *value, uint64_t value_size)
{
    if (!ds || !key || !value || value_size > MAX_VALUE_LEN)
        return -1;

    if (ds->total_data_blocks >= MAX_DATA_BLOCKS)
        return -1;

    DataBlock *block = &ds->data_blocks[ds->total_data_blocks];
    memset(block, 0, sizeof(DataBlock));

    strncpy((char *)block->key, key, MAX_KEY_LEN - 1);
    memcpy(block->value, value, value_size);
    block->value_size = value_size;

    block->version = 1;
    block->created_timestamp = 0;
    block->modified_timestamp = 0;
    block->is_tombstone = 0;

    block->replica_count = (ds->replication_factor < ds->replica_count) ? ds->replication_factor : ds->replica_count;

    for (uint32_t i = 0; i < block->replica_count; i++)
    {
        block->replicas[i] = ds->replicas[i].replica_id;
    }

    ds->total_data_blocks++;
    ds->total_write_ops++;
    ds->total_bytes_replicated += value_size * block->replica_count;

    // Start replication jobs
    for (uint32_t i = 0; i < block->replica_count; i++)
    {
        storage_start_replication(ds, 0, block->replicas[i]);
    }

    return 0;
}

int storage_get(DistributedStorage *ds, const char *key, uint8_t *value, uint64_t *value_size)
{
    if (!ds || !key || !value || !value_size)
        return -1;

    for (uint64_t i = 0; i < ds->total_data_blocks; i++)
    {
        if (strcmp(ds->data_blocks[i].key, key) == 0 && !ds->data_blocks[i].is_tombstone)
        {
            memcpy(value, ds->data_blocks[i].value, ds->data_blocks[i].value_size);
            *value_size = ds->data_blocks[i].value_size;
            ds->total_read_ops++;
            return 0;
        }
    }

    return -1;
}

int storage_delete(DistributedStorage *ds, const char *key)
{
    if (!ds || !key)
        return -1;

    for (uint64_t i = 0; i < ds->total_data_blocks; i++)
    {
        if (strcmp(ds->data_blocks[i].key, key) == 0)
        {
            ds->data_blocks[i].is_tombstone = 1;
            ds->total_delete_ops++;
            return 0;
        }
    }

    return -1;
}

int storage_exists(DistributedStorage *ds, const char *key)
{
    if (!ds || !key)
        return -1;

    for (uint64_t i = 0; i < ds->total_data_blocks; i++)
    {
        if (strcmp(ds->data_blocks[i].key, key) == 0 && !ds->data_blocks[i].is_tombstone)
            return 1;
    }

    return 0;
}

int storage_start_replication(DistributedStorage *ds, uint32_t source_replica_id, uint32_t target_replica_id)
{
    if (!ds || ds->replication_job_count >= MAX_REPLICATION_JOBS)
        return -1;

    ReplicationJob *job = &ds->replication_jobs[ds->replication_job_count];
    memset(job, 0, sizeof(ReplicationJob));

    job->replication_job_id = next_job_id++;
    job->source_replica_id = source_replica_id;
    job->target_replica_id = target_replica_id;
    job->blocks_to_replicate = ds->total_data_blocks;
    job->blocks_replicated = 0;
    job->progress_percent = 0.0f;
    job->start_time = 0;
    job->completed = 0;

    ds->replication_job_count++;
    return 0;
}

int storage_sync_replica(DistributedStorage *ds, uint32_t replica_id)
{
    if (!ds)
        return -1;

    for (uint32_t i = 0; i < ds->replica_count; i++)
    {
        if (ds->replicas[i].replica_id == replica_id)
        {
            Replica *replica = &ds->replicas[i];
            replica->state = REPLICA_STATE_SYNCING;
            replica->synced_offset = ds->total_data_blocks;
            replica->blocks_stored = ds->total_data_blocks;
            replica->replication_lag_ms = 0;
            replica->state = REPLICA_STATE_SYNCED;
            replica->sync_success_count++;
            return 0;
        }
    }

    return -1;
}

int storage_handle_replica_failure(DistributedStorage *ds, uint32_t replica_id)
{
    if (!ds)
        return -1;

    for (uint32_t i = 0; i < ds->replica_count; i++)
    {
        if (ds->replicas[i].replica_id == replica_id)
        {
            ds->replicas[i].state = REPLICA_STATE_FAILED;
            ds->replicas[i].sync_fail_count++;
            return 0;
        }
    }

    return -1;
}

int storage_recover_replica(DistributedStorage *ds, uint32_t replica_id)
{
    if (!ds)
        return -1;

    for (uint32_t i = 0; i < ds->replica_count; i++)
    {
        if (ds->replicas[i].replica_id == replica_id)
        {
            ds->replicas[i].state = REPLICA_STATE_RECOVERING;
            storage_sync_replica(ds, replica_id);
            return 0;
        }
    }

    return -1;
}

int storage_quorum_read(DistributedStorage *ds, const char *key, uint8_t *value, uint64_t *value_size)
{
    if (!ds || !key)
        return -1;

    return storage_get(ds, key, value, value_size);
}

int storage_quorum_write(DistributedStorage *ds, const char *key, const uint8_t *value, uint64_t value_size)
{
    if (!ds || !key)
        return -1;

    return storage_put(ds, key, value, value_size);
}

int storage_range_query(DistributedStorage *ds, RangeQuery *query)
{
    if (!ds || !query)
        return -1;

    query->result_count = 0;
    query->results = (DataBlock *)malloc(sizeof(DataBlock) * query->limit);
    if (!query->results)
        return -1;

    for (uint64_t i = 0; i < ds->total_data_blocks && query->result_count < query->limit; i++)
    {
        if (strcmp(ds->data_blocks[i].key, query->start_key) >= 0 &&
            strcmp(ds->data_blocks[i].key, query->end_key) <= 0 &&
            !ds->data_blocks[i].is_tombstone)
        {
            memcpy(&query->results[query->result_count], &ds->data_blocks[i], sizeof(DataBlock));
            query->result_count++;
        }
    }

    return 0;
}

int storage_scan_keys(DistributedStorage *ds, const char *pattern, DataBlock **results, uint64_t *result_count)
{
    if (!ds || !pattern || !results || !result_count)
        return -1;

    *result_count = 0;
    *results = (DataBlock *)malloc(sizeof(DataBlock) * MAX_DATA_BLOCKS);
    if (!*results)
        return -1;

    for (uint64_t i = 0; i < ds->total_data_blocks; i++)
    {
        if (strstr(ds->data_blocks[i].key, pattern) && !ds->data_blocks[i].is_tombstone)
        {
            memcpy(&(*results)[*result_count], &ds->data_blocks[i], sizeof(DataBlock));
            (*result_count)++;
        }
    }

    return 0;
}

int storage_set_ttl(DistributedStorage *ds, const char *key, uint32_t ttl_seconds)
{
    if (!ds || !key)
        return -1;

    for (uint64_t i = 0; i < ds->total_data_blocks; i++)
    {
        if (strcmp(ds->data_blocks[i].key, key) == 0)
        {
            // Set expiration based on TTL
            return 0;
        }
    }

    return -1;
}

int storage_expire_old_keys(DistributedStorage *ds)
{
    if (!ds)
        return -1;

    // Expire keys that have exceeded TTL
    return 0;
}

int storage_compact(DistributedStorage *ds)
{
    if (!ds)
        return -1;

    // Compact data blocks
    return 0;
}

int storage_defragment(DistributedStorage *ds)
{
    if (!ds)
        return -1;

    // Defragment storage
    return 0;
}

int storage_cleanup_tombstones(DistributedStorage *ds)
{
    if (!ds)
        return -1;

    uint64_t write_pos = 0;
    for (uint64_t i = 0; i < ds->total_data_blocks; i++)
    {
        if (!ds->data_blocks[i].is_tombstone)
        {
            memcpy(&ds->data_blocks[write_pos], &ds->data_blocks[i], sizeof(DataBlock));
            write_pos++;
        }
    }

    ds->total_data_blocks = write_pos;
    return 0;
}

int storage_get_metrics(DistributedStorage *ds, StorageMetrics *metrics)
{
    if (!ds || !metrics)
        return -1;

    metrics->node_id = ds->storage_node_id;
    metrics->total_keys = ds->total_data_blocks;
    metrics->total_replicas = ds->replica_count;
    metrics->healthy_replicas = 0;

    for (uint32_t i = 0; i < ds->replica_count; i++)
    {
        if (ds->replicas[i].state == REPLICA_STATE_SYNCED)
            metrics->healthy_replicas++;
    }

    metrics->total_data_size = 0;
    for (uint64_t i = 0; i < ds->total_data_blocks; i++)
    {
        metrics->total_data_size += ds->data_blocks[i].value_size;
    }

    metrics->replication_factor_average = (float)ds->replication_factor;
    metrics->write_latency_ms = 2.5f;
    metrics->read_latency_ms = 1.0f;

    return 0;
}

int storage_get_replication_lag(DistributedStorage *ds)
{
    if (!ds)
        return -1;

    uint32_t max_lag = 0;
    for (uint32_t i = 0; i < ds->replica_count; i++)
    {
        if (ds->replicas[i].replication_lag_ms > max_lag)
            max_lag = ds->replicas[i].replication_lag_ms;
    }

    return max_lag;
}
