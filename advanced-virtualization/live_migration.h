#ifndef LIVE_MIGRATION_H
#define LIVE_MIGRATION_H

#include <stdint.h>
#include <stddef.h>

#define MAX_MIGRATION_SESSIONS 64
#define MAX_DIRTY_PAGE_LOG 32768

typedef enum
{
    MIGRATION_PRE_COPY,
    MIGRATION_POST_COPY,
    MIGRATION_HYBRID
} MigrationStrategy;

typedef enum
{
    MIGRATION_INIT,
    MIGRATION_SETUP,
    MIGRATION_DELTA,
    MIGRATION_STOP_VM,
    MIGRATION_FINAL,
    MIGRATION_ABORT,
    MIGRATION_COMPLETE
} MigrationPhase;

typedef struct
{
    uint64_t page_address;
    uint64_t iteration;
    uint8_t is_dirty;
} DirtyPageLog;

typedef struct
{
    uint32_t migration_session_id;
    uint32_t source_vm_id;
    uint32_t dest_vm_id;

    MigrationStrategy strategy;
    MigrationPhase current_phase;

    uint64_t total_pages;
    uint64_t migrated_pages;
    uint64_t dirty_pages;
    uint64_t remaining_pages;

    DirtyPageLog dirty_logs[MAX_DIRTY_PAGE_LOG];
    uint32_t dirty_log_count;

    uint64_t start_time;
    uint64_t end_time;
    uint64_t total_migration_time_ms;

    float progress_percent;
    float estimated_time_remaining_ms;

    uint8_t can_rollback;
    uint32_t iteration_count;
} MigrationSession;

typedef struct
{
    uint32_t manager_id;
    uint32_t active_migrations;
    uint32_t completed_migrations;
    uint32_t failed_migrations;

    MigrationSession sessions[MAX_MIGRATION_SESSIONS];

    uint64_t total_pages_migrated;
    uint64_t total_downtime_ms;

    float average_migration_time_ms;
    float average_downtime_ms;
} LiveMigrationManager;

LiveMigrationManager *live_migration_init(void);
int live_migration_destroy(LiveMigrationManager *manager);

int migration_session_create(LiveMigrationManager *manager, uint32_t source_vm, uint32_t dest_vm, MigrationStrategy strategy);
int migration_session_destroy(LiveMigrationManager *manager, uint32_t session_id);

int migration_start(LiveMigrationManager *manager, uint32_t session_id);
int migration_precopy_phase(LiveMigrationManager *manager, uint32_t session_id);
int migration_delta_phase(LiveMigrationManager *manager, uint32_t session_id);
int migration_stop_vm_phase(LiveMigrationManager *manager, uint32_t session_id);
int migration_final_phase(LiveMigrationManager *manager, uint32_t session_id);
int migration_complete(LiveMigrationManager *manager, uint32_t session_id);

int migration_track_dirty_pages(LiveMigrationManager *manager, uint32_t session_id, uint64_t page_address);
int migration_clear_dirty_log(LiveMigrationManager *manager, uint32_t session_id);

int migration_calculate_progress(LiveMigrationManager *manager, uint32_t session_id);
int migration_handle_timeout(LiveMigrationManager *manager, uint32_t session_id);
int migration_rollback(LiveMigrationManager *manager, uint32_t session_id);

typedef struct
{
    uint32_t manager_id;
    uint32_t active_migrations;
    uint32_t completed_migrations;
    uint32_t failed_migrations;
    float average_downtime_ms;
    uint64_t total_pages_migrated;
} MigrationMetrics;

int live_migration_get_metrics(LiveMigrationManager *manager, MigrationMetrics *metrics);

#endif
