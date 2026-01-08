#ifndef VM_LIFECYCLE_H
#define VM_LIFECYCLE_H

#include <stdint.h>
#include <stddef.h>

#define MAX_SNAPSHOTS 32
#define MAX_BACKUP_JOBS 16
#define MAX_MIGRATION_JOBS 8

typedef enum
{
    SNAPSHOT_STATE_ACTIVE,
    SNAPSHOT_STATE_SAVED,
    SNAPSHOT_STATE_RESTORING,
    SNAPSHOT_STATE_DELETED
} SnapshotState;

typedef enum
{
    BACKUP_STATE_PENDING,
    BACKUP_STATE_RUNNING,
    BACKUP_STATE_COMPLETED,
    BACKUP_STATE_FAILED,
    BACKUP_STATE_VERIFIED
} BackupState;

typedef enum
{
    MIGRATION_STATE_INITIATED,
    MIGRATION_STATE_TRANSFERRING,
    MIGRATION_STATE_ACTIVATING,
    MIGRATION_STATE_COMPLETED,
    MIGRATION_STATE_FAILED,
    MIGRATION_STATE_ROLLED_BACK
} MigrationState;

typedef enum
{
    MIGRATION_TYPE_LIVE,
    MIGRATION_TYPE_COLD,
    MIGRATION_TYPE_WARM,
    MIGRATION_TYPE_STORAGE_ONLY
} MigrationType;

typedef struct
{
    uint32_t snapshot_id;
    char name[64];
    uint32_t vm_id;

    uint64_t memory_snapshot_size;
    uint64_t disk_snapshot_size;
    uint64_t total_snapshot_size;

    SnapshotState state;
    uint32_t created_timestamp;
    uint32_t parent_snapshot_id;

    void *memory_data;
    void *disk_data;
} VMSnapshot;

typedef struct
{
    uint32_t backup_id;
    char backup_name[64];
    uint32_t vm_id;

    BackupState state;
    uint32_t created_timestamp;
    uint32_t completed_timestamp;

    uint64_t data_size;
    char backup_location[256];

    uint8_t compressed;
    uint8_t encrypted;
    char encryption_algorithm[32];
} VMBackup;

typedef struct
{
    uint32_t migration_id;
    uint32_t source_vm_id;
    uint32_t destination_hypervisor_id;

    MigrationType migration_type;
    MigrationState state;

    uint32_t start_time;
    uint32_t end_time;

    uint64_t bytes_transferred;
    uint64_t total_bytes;
    float progress_percentage;

    uint32_t downtime_milliseconds;
} VMMigration;

typedef struct
{
    uint32_t lifecycle_manager_id;

    uint32_t snapshot_count;
    VMSnapshot snapshots[MAX_SNAPSHOTS];

    uint32_t backup_count;
    VMBackup backups[MAX_BACKUP_JOBS];

    uint32_t migration_count;
    VMMigration migrations[MAX_MIGRATION_JOBS];

    uint64_t total_backup_size;
    uint64_t total_migration_count;
    uint64_t successful_migrations;
    uint64_t failed_migrations;
} VMLifecycleManager;

// Lifecycle Manager initialization
VMLifecycleManager *vm_lifecycle_manager_init(void);
int vm_lifecycle_manager_destroy(VMLifecycleManager *lcm);

// Snapshot operations
VMSnapshot *vm_create_snapshot(VMLifecycleManager *lcm, uint32_t vm_id, const char *snapshot_name);
int vm_restore_snapshot(VMLifecycleManager *lcm, uint32_t snapshot_id);
int vm_delete_snapshot(VMLifecycleManager *lcm, uint32_t snapshot_id);
int vm_merge_snapshots(VMLifecycleManager *lcm, uint32_t parent_snapshot_id, uint32_t child_snapshot_id);

// Backup operations
VMBackup *vm_create_backup(VMLifecycleManager *lcm, uint32_t vm_id, const char *backup_name);
int vm_restore_backup(VMLifecycleManager *lcm, uint32_t backup_id, uint32_t target_vm_id);
int vm_delete_backup(VMLifecycleManager *lcm, uint32_t backup_id);
int vm_verify_backup(VMLifecycleManager *lcm, uint32_t backup_id);

// Migration operations
VMMigration *vm_create_migration(VMLifecycleManager *lcm, uint32_t source_vm_id,
                                 uint32_t dest_hypervisor_id, MigrationType migration_type);
int vm_start_migration(VMMigration *migration);
int vm_cancel_migration(VMMigration *migration);
int vm_rollback_migration(VMMigration *migration);

// Scheduling and automation
typedef struct
{
    uint32_t schedule_id;
    uint32_t vm_id;
    uint32_t trigger_interval; // seconds
    char action[32];           // snapshot, backup, etc
} ScheduledAction;

int vm_schedule_action(VMLifecycleManager *lcm, uint32_t vm_id, const char *action, uint32_t interval);
int vm_unschedule_action(VMLifecycleManager *lcm, uint32_t schedule_id);

// Disaster recovery
int vm_trigger_dr_plan(VMLifecycleManager *lcm, uint32_t vm_id);
int vm_get_recovery_point_objective(VMLifecycleManager *lcm, uint32_t vm_id);
int vm_get_recovery_time_objective(VMLifecycleManager *lcm, uint32_t vm_id);

// Monitoring and reporting
typedef struct
{
    uint32_t vm_id;
    uint64_t total_backup_size;
    uint64_t total_snapshot_size;
    float backup_success_rate;
    uint32_t last_backup_timestamp;
    uint32_t last_snapshot_timestamp;
} VMLifecycleMetrics;

int vm_get_lifecycle_metrics(VMLifecycleManager *lcm, uint32_t vm_id, VMLifecycleMetrics *metrics);
int vm_generate_lifecycle_report(VMLifecycleManager *lcm);

#endif // VM_LIFECYCLE_H
