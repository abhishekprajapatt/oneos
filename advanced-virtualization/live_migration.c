#include "live_migration.h"
#include <stdlib.h>
#include <string.h>

static LiveMigrationManager *global_live_migration = NULL;

LiveMigrationManager *live_migration_init(void)
{
    LiveMigrationManager *manager = (LiveMigrationManager *)malloc(sizeof(LiveMigrationManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(LiveMigrationManager));
    manager->manager_id = 1;
    manager->active_migrations = 0;
    manager->completed_migrations = 0;
    manager->failed_migrations = 0;
    manager->total_pages_migrated = 0;
    manager->total_downtime_ms = 0;
    manager->average_migration_time_ms = 0.0f;
    manager->average_downtime_ms = 0.0f;

    global_live_migration = manager;
    return manager;
}

int live_migration_destroy(LiveMigrationManager *manager)
{
    if (!manager)
        return -1;

    free(manager);
    global_live_migration = NULL;
    return 0;
}

int migration_session_create(LiveMigrationManager *manager, uint32_t source_vm, uint32_t dest_vm, MigrationStrategy strategy)
{
    if (!manager || manager->active_migrations >= MAX_MIGRATION_SESSIONS)
        return -1;

    MigrationSession *session = NULL;
    uint32_t session_id = 0;

    for (uint32_t i = 0; i < MAX_MIGRATION_SESSIONS; i++)
    {
        if (manager->sessions[i].migration_session_id == 0)
        {
            session = &manager->sessions[i];
            session_id = i + 1;
            break;
        }
    }

    if (!session)
        return -1;

    memset(session, 0, sizeof(MigrationSession));
    session->migration_session_id = session_id;
    session->source_vm_id = source_vm;
    session->dest_vm_id = dest_vm;
    session->strategy = strategy;
    session->current_phase = MIGRATION_INIT;
    session->total_pages = 131072;
    session->migrated_pages = 0;
    session->dirty_pages = 0;
    session->remaining_pages = session->total_pages;
    session->dirty_log_count = 0;
    session->start_time = 0;
    session->end_time = 0;
    session->total_migration_time_ms = 0;
    session->progress_percent = 0.0f;
    session->estimated_time_remaining_ms = 0.0f;
    session->can_rollback = 1;
    session->iteration_count = 0;

    manager->active_migrations++;
    return session_id;
}

int migration_session_destroy(LiveMigrationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0 || session_id > MAX_MIGRATION_SESSIONS)
        return -1;

    MigrationSession *session = &manager->sessions[session_id - 1];
    if (session->migration_session_id == 0)
        return -1;

    memset(session, 0, sizeof(MigrationSession));
    if (manager->active_migrations > 0)
        manager->active_migrations--;

    return 0;
}

int migration_start(LiveMigrationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0 || session_id > MAX_MIGRATION_SESSIONS)
        return -1;

    MigrationSession *session = &manager->sessions[session_id - 1];
    if (session->migration_session_id == 0)
        return -1;

    session->current_phase = MIGRATION_SETUP;
    session->start_time = 0;
    return 0;
}

int migration_precopy_phase(LiveMigrationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0 || session_id > MAX_MIGRATION_SESSIONS)
        return -1;

    MigrationSession *session = &manager->sessions[session_id - 1];
    if (session->migration_session_id == 0)
        return -1;

    session->current_phase = MIGRATION_DELTA;
    session->iteration_count++;

    uint64_t pages_to_migrate = (session->total_pages * 20) / 100;
    session->migrated_pages += pages_to_migrate;
    session->remaining_pages -= pages_to_migrate;

    session->progress_percent = ((float)session->migrated_pages / session->total_pages) * 100.0f;

    return pages_to_migrate;
}

int migration_delta_phase(LiveMigrationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0 || session_id > MAX_MIGRATION_SESSIONS)
        return -1;

    MigrationSession *session = &manager->sessions[session_id - 1];
    if (session->migration_session_id == 0)
        return -1;

    session->dirty_pages = (session->total_pages * 5) / 100;

    uint64_t delta_pages = (session->dirty_pages > 0) ? session->dirty_pages : 1;
    session->migrated_pages += delta_pages;
    session->remaining_pages -= delta_pages;

    session->progress_percent = ((float)session->migrated_pages / session->total_pages) * 100.0f;
    session->iteration_count++;

    return delta_pages;
}

int migration_stop_vm_phase(LiveMigrationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0 || session_id > MAX_MIGRATION_SESSIONS)
        return -1;

    MigrationSession *session = &manager->sessions[session_id - 1];
    if (session->migration_session_id == 0)
        return -1;

    session->current_phase = MIGRATION_STOP_VM;
    return 0;
}

int migration_final_phase(LiveMigrationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0 || session_id > MAX_MIGRATION_SESSIONS)
        return -1;

    MigrationSession *session = &manager->sessions[session_id - 1];
    if (session->migration_session_id == 0)
        return -1;

    session->current_phase = MIGRATION_FINAL;

    session->migrated_pages = session->total_pages;
    session->remaining_pages = 0;
    session->progress_percent = 100.0f;

    return 0;
}

int migration_complete(LiveMigrationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0 || session_id > MAX_MIGRATION_SESSIONS)
        return -1;

    MigrationSession *session = &manager->sessions[session_id - 1];
    if (session->migration_session_id == 0)
        return -1;

    session->current_phase = MIGRATION_COMPLETE;
    session->end_time = 0;
    session->total_migration_time_ms = (session->end_time > session->start_time) ? (session->end_time - session->start_time) : 100;

    manager->total_pages_migrated += session->total_pages;
    manager->completed_migrations++;

    if (manager->active_migrations > 0)
        manager->active_migrations--;

    return 0;
}

int migration_track_dirty_pages(LiveMigrationManager *manager, uint32_t session_id, uint64_t page_address)
{
    if (!manager || session_id == 0 || session_id > MAX_MIGRATION_SESSIONS)
        return -1;

    MigrationSession *session = &manager->sessions[session_id - 1];
    if (session->migration_session_id == 0)
        return -1;

    if (session->dirty_log_count >= MAX_DIRTY_PAGE_LOG)
        return -1;

    DirtyPageLog *log = &session->dirty_logs[session->dirty_log_count];
    log->page_address = page_address;
    log->iteration = session->iteration_count;
    log->is_dirty = 1;

    session->dirty_log_count++;
    return 0;
}

int migration_clear_dirty_log(LiveMigrationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0 || session_id > MAX_MIGRATION_SESSIONS)
        return -1;

    MigrationSession *session = &manager->sessions[session_id - 1];
    if (session->migration_session_id == 0)
        return -1;

    session->dirty_log_count = 0;
    memset(session->dirty_logs, 0, sizeof(session->dirty_logs));
    return 0;
}

int migration_calculate_progress(LiveMigrationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0 || session_id > MAX_MIGRATION_SESSIONS)
        return -1;

    MigrationSession *session = &manager->sessions[session_id - 1];
    if (session->migration_session_id == 0)
        return -1;

    if (session->total_pages > 0)
    {
        session->progress_percent = ((float)session->migrated_pages / session->total_pages) * 100.0f;
    }

    if (session->remaining_pages > 0 && session->iteration_count > 0)
    {
        session->estimated_time_remaining_ms = (session->total_migration_time_ms * session->remaining_pages) / session->migrated_pages;
    }
    else
    {
        session->estimated_time_remaining_ms = 0.0f;
    }

    return 0;
}

int migration_handle_timeout(LiveMigrationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0 || session_id > MAX_MIGRATION_SESSIONS)
        return -1;

    MigrationSession *session = &manager->sessions[session_id - 1];
    if (session->migration_session_id == 0)
        return -1;

    if (session->can_rollback)
    {
        return migration_rollback(manager, session_id);
    }

    return -1;
}

int migration_rollback(LiveMigrationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0 || session_id > MAX_MIGRATION_SESSIONS)
        return -1;

    MigrationSession *session = &manager->sessions[session_id - 1];
    if (session->migration_session_id == 0)
        return -1;

    session->current_phase = MIGRATION_ABORT;
    manager->failed_migrations++;

    if (manager->active_migrations > 0)
        manager->active_migrations--;

    return 0;
}

int live_migration_get_metrics(LiveMigrationManager *manager, MigrationMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    metrics->manager_id = manager->manager_id;
    metrics->active_migrations = manager->active_migrations;
    metrics->completed_migrations = manager->completed_migrations;
    metrics->failed_migrations = manager->failed_migrations;
    metrics->total_pages_migrated = manager->total_pages_migrated;

    if (manager->completed_migrations > 0)
    {
        metrics->average_downtime_ms = (float)manager->total_downtime_ms / manager->completed_migrations;
    }
    else
    {
        metrics->average_downtime_ms = 0.0f;
    }

    return 0;
}
