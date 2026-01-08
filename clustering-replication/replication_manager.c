#include "replication_manager.h"
#include <stdlib.h>
#include <string.h>

static ReplicationManager *global_replication_manager = NULL;
static uint32_t next_session_id = 1;
static uint32_t next_replica_id = 1;
static uint32_t next_log_entry_id = 1;

ReplicationManager *replication_manager_init(void)
{
    ReplicationManager *manager = (ReplicationManager *)malloc(sizeof(ReplicationManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(ReplicationManager));
    manager->manager_id = 1;
    manager->session_count = 0;
    manager->total_replicated_data = 0;
    manager->total_replication_operations = 0;
    manager->replication_failures = 0;
    manager->replication_efficiency = 0.0f;

    global_replication_manager = manager;
    return manager;
}

int replication_manager_destroy(ReplicationManager *manager)
{
    if (!manager)
        return -1;

    free(manager);
    global_replication_manager = NULL;
    return 0;
}

uint32_t replication_create_session(ReplicationManager *manager, uint32_t source_node_id,
                                    ReplicationStrategy strategy, ConsistencyModel consistency)
{
    if (!manager || manager->session_count >= MAX_REPLICATION_SESSIONS)
        return 0;

    ReplicationSession *session = &manager->sessions[manager->session_count];
    memset(session, 0, sizeof(ReplicationSession));

    session->session_id = next_session_id++;
    session->source_node_id = source_node_id;
    session->replica_count = 0;
    session->log_size = 0;
    session->log_head = 0;
    session->log_tail = 0;
    session->strategy = strategy;
    session->consistency = consistency;
    session->total_bytes_replicated = 0;
    session->total_replication_events = 0;

    manager->session_count++;
    return session->session_id;
}

int replication_destroy_session(ReplicationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            memmove(&manager->sessions[i], &manager->sessions[i + 1],
                    sizeof(ReplicationSession) * (manager->session_count - i - 1));
            manager->session_count--;
            return 0;
        }
    }

    return -1;
}

int replication_add_replica(ReplicationManager *manager, uint32_t session_id,
                            uint32_t replica_id, uint32_t node_id)
{
    if (!manager || session_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            ReplicationSession *session = &manager->sessions[i];

            if (session->replica_count >= MAX_REPLICAS)
                return -1;

            ReplicaInfo *replica = &session->replicas[session->replica_count];
            memset(replica, 0, sizeof(ReplicaInfo));

            replica->replica_id = replica_id;
            replica->node_id = node_id;
            replica->state = REPLICA_STATE_OUT_OF_SYNC;
            replica->last_sync_time = 0;
            replica->sync_lag_ms = 0;
            replica->bytes_replicated = 0;
            replica->successful_syncs = 0;
            replica->failed_syncs = 0;
            replica->sync_success_rate = 0.0f;

            session->replica_count++;
            return 0;
        }
    }

    return -1;
}

int replication_remove_replica(ReplicationManager *manager, uint32_t session_id, uint32_t replica_id)
{
    if (!manager || session_id == 0 || replica_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            ReplicationSession *session = &manager->sessions[i];

            for (uint32_t j = 0; j < session->replica_count; j++)
            {
                if (session->replicas[j].replica_id == replica_id)
                {
                    memmove(&session->replicas[j], &session->replicas[j + 1],
                            sizeof(ReplicaInfo) * (session->replica_count - j - 1));
                    session->replica_count--;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int replication_sync_replica(ReplicationManager *manager, uint32_t session_id, uint32_t replica_id)
{
    if (!manager || session_id == 0 || replica_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            ReplicationSession *session = &manager->sessions[i];

            for (uint32_t j = 0; j < session->replica_count; j++)
            {
                if (session->replicas[j].replica_id == replica_id)
                {
                    ReplicaInfo *replica = &session->replicas[j];
                    replica->state = REPLICA_STATE_SYNCING;
                    replica->last_sync_time = 0;
                    replica->sync_lag_ms = 0;
                    replica->successful_syncs++;

                    if (replica->successful_syncs > 0 && replica->failed_syncs == 0)
                    {
                        replica->sync_success_rate = 100.0f;
                    }

                    replica->state = REPLICA_STATE_SYNCED;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int replication_detect_out_of_sync(ReplicationManager *manager, uint32_t session_id, uint32_t replica_id)
{
    if (!manager || session_id == 0 || replica_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            ReplicationSession *session = &manager->sessions[i];

            for (uint32_t j = 0; j < session->replica_count; j++)
            {
                if (session->replicas[j].replica_id == replica_id)
                {
                    ReplicaInfo *replica = &session->replicas[j];
                    replica->state = REPLICA_STATE_OUT_OF_SYNC;
                    replica->sync_lag_ms = 1000;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int replication_log_entry_add(ReplicationManager *manager, uint32_t session_id,
                              const uint8_t *data, uint32_t data_size, uint32_t source_node_id)
{
    if (!manager || session_id == 0 || !data || data_size == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            ReplicationSession *session = &manager->sessions[i];

            if (session->log_size >= MAX_REPLICATION_LOG)
                return -1;

            uint32_t idx = session->log_tail;
            ReplicationLogEntry *entry = &session->log[idx];
            memset(entry, 0, sizeof(ReplicationLogEntry));

            entry->log_entry_id = next_log_entry_id++;
            if (data_size > sizeof(entry->data))
                data_size = sizeof(entry->data);

            memcpy(entry->data, data, data_size);
            entry->data_size = data_size;
            entry->timestamp = 0;
            entry->source_node_id = source_node_id;
            entry->replication_count = 0;

            session->log_tail = (session->log_tail + 1) % MAX_REPLICATION_LOG;
            session->log_size++;
            session->total_bytes_replicated += data_size;
            session->total_replication_events++;
            manager->total_replicated_data += data_size;
            manager->total_replication_operations++;

            return entry->log_entry_id;
        }
    }

    return -1;
}

int replication_apply_log_entry(ReplicationManager *manager, uint32_t session_id, uint32_t log_id)
{
    if (!manager || session_id == 0 || log_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            ReplicationSession *session = &manager->sessions[i];

            for (uint32_t j = 0; j < session->log_size; j++)
            {
                if (session->log[j].log_entry_id == log_id)
                {
                    session->log[j].replication_count++;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int replication_start_sync(ReplicationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            return 0;
        }
    }

    return -1;
}

int replication_complete_sync(ReplicationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            ReplicationSession *session = &manager->sessions[i];

            for (uint32_t j = 0; j < session->replica_count; j++)
            {
                session->replicas[j].state = REPLICA_STATE_SYNCED;
            }

            return 0;
        }
    }

    return -1;
}

int replication_check_consistency(ReplicationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            ReplicationSession *session = &manager->sessions[i];
            uint32_t synced_count = 0;

            for (uint32_t j = 0; j < session->replica_count; j++)
            {
                if (session->replicas[j].state == REPLICA_STATE_SYNCED)
                {
                    synced_count++;
                }
            }

            if (synced_count == session->replica_count)
            {
                return 1;
            }

            return 0;
        }
    }

    return -1;
}

int replication_resolve_conflict(ReplicationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0)
        return -1;

    return 0;
}

int replication_update_replica_state(ReplicationManager *manager, uint32_t session_id,
                                     uint32_t replica_id, ReplicaState state)
{
    if (!manager || session_id == 0 || replica_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            ReplicationSession *session = &manager->sessions[i];

            for (uint32_t j = 0; j < session->replica_count; j++)
            {
                if (session->replicas[j].replica_id == replica_id)
                {
                    session->replicas[j].state = state;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int replication_get_metrics(ReplicationManager *manager, ReplicationMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    uint32_t synced_replicas = 0;
    uint32_t total_replicas = 0;
    uint32_t successful_ops = 0;
    uint32_t failed_ops = 0;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        ReplicationSession *session = &manager->sessions[i];
        total_replicas += session->replica_count;

        for (uint32_t j = 0; j < session->replica_count; j++)
        {
            if (session->replicas[j].state == REPLICA_STATE_SYNCED)
            {
                synced_replicas++;
            }
            successful_ops += session->replicas[j].successful_syncs;
            failed_ops += session->replicas[j].failed_syncs;
        }
    }

    metrics->manager_id = manager->manager_id;
    metrics->active_sessions = manager->session_count;
    metrics->total_replicas = total_replicas;
    metrics->synced_replicas = synced_replicas;
    metrics->total_replicated_bytes = manager->total_replicated_data;
    metrics->replication_operations = manager->total_replication_operations;

    if (successful_ops + failed_ops > 0)
    {
        metrics->replication_success_rate = (float)successful_ops / (successful_ops + failed_ops);
    }
    else
    {
        metrics->replication_success_rate = 0.0f;
    }

    return 0;
}
