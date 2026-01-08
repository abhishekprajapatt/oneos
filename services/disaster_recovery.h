#ifndef DISASTER_RECOVERY_H
#define DISASTER_RECOVERY_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>

#define MAX_DR_MANAGERS 8
#define MAX_BACKUP_POLICIES 64
#define MAX_RESTORE_POINTS 1024
#define MAX_FAILOVER_CONFIGS 128
#define MAX_REPLICATED_SYSTEMS 256
#define MAX_RECOVERY_PLANS 32

typedef enum
{
    BACKUP_FULL = 1,
    BACKUP_INCREMENTAL = 2,
    BACKUP_DIFFERENTIAL = 3,
    BACKUP_SNAPSHOT = 4
} BackupType;

typedef enum
{
    RESTORE_POINT_HOURLY = 1,
    RESTORE_POINT_DAILY = 2,
    RESTORE_POINT_WEEKLY = 3,
    RESTORE_POINT_MONTHLY = 4,
    RESTORE_POINT_CUSTOM = 5
} RestorePointType;

typedef enum
{
    FAILOVER_MANUAL = 1,
    FAILOVER_AUTOMATIC = 2,
    FAILOVER_SEMI_AUTOMATIC = 3
} FailoverType;

typedef enum
{
    SYSTEM_ONLINE = 1,
    SYSTEM_DEGRADED = 2,
    SYSTEM_OFFLINE = 3,
    SYSTEM_RECOVERING = 4,
    SYSTEM_RECOVERED = 5
} SystemStatus;

typedef struct
{
    uint32_t policy_id;
    char policy_name[64];
    BackupType backup_type;
    uint32_t retention_days;
    uint32_t frequency_hours;
    uint32_t compression_enabled;
    uint32_t encryption_enabled;
    uint32_t is_enabled;
} BackupPolicy;

typedef struct
{
    uint64_t restore_point_id;
    uint32_t policy_id;
    RestorePointType point_type;
    uint64_t created_at;
    uint64_t backup_size_bytes;
    char backup_location[256];
    char checksum[64];
    uint32_t is_verified;
    uint32_t is_available;
} RestorePoint;

typedef struct
{
    uint32_t system_id;
    char system_name[64];
    char primary_address[256];
    char secondary_address[256];
    SystemStatus primary_status;
    SystemStatus secondary_status;
    uint64_t last_sync_time;
    uint64_t replication_lag_bytes;
    uint32_t auto_failover_enabled;
} ReplicatedSystem;

typedef struct
{
    uint32_t failover_id;
    char failover_name[64];
    FailoverType failover_type;
    uint32_t system_id;
    uint32_t rto_seconds;
    uint32_t rpo_seconds;
    uint32_t is_enabled;
    uint64_t last_failover_time;
    uint32_t failover_count;
} FailoverConfig;

typedef struct
{
    uint32_t plan_id;
    char plan_name[128];
    uint32_t system_ids[32];
    uint32_t system_count;
    char recovery_steps[512];
    uint32_t estimated_recovery_time_hours;
    uint64_t created_at;
    uint64_t last_tested_at;
} RecoveryPlan;

typedef struct
{
    uint32_t dr_manager_id;
    char name[64];

    BackupPolicy backup_policies[MAX_BACKUP_POLICIES];
    uint32_t policy_count;

    RestorePoint restore_points[MAX_RESTORE_POINTS];
    uint32_t restore_point_count;

    ReplicatedSystem replicated_systems[MAX_REPLICATED_SYSTEMS];
    uint32_t system_count;

    FailoverConfig failover_configs[MAX_FAILOVER_CONFIGS];
    uint32_t failover_count;

    RecoveryPlan recovery_plans[MAX_RECOVERY_PLANS];
    uint32_t plan_count;

    uint32_t global_rto_seconds;
    uint32_t global_rpo_seconds;

    uint64_t total_backups_created;
    uint64_t total_restores_performed;
    uint64_t total_failovers_executed;
    uint64_t total_backup_bytes;
} DisasterRecoveryManager;

int dr_create_manager(const char *name, uint32_t rto_sec, uint32_t rpo_sec);
int dr_delete_manager(uint32_t dr_id);

int dr_add_backup_policy(uint32_t dr_id, const char *policy_name, BackupType backup_type,
                         uint32_t retention_days, uint32_t frequency_hours);
int dr_remove_backup_policy(uint32_t dr_id, uint32_t policy_id);
int dr_execute_backup(uint32_t dr_id, uint32_t policy_id, const char *backup_location);

int dr_create_restore_point(uint32_t dr_id, uint32_t policy_id, RestorePointType point_type,
                            const char *backup_location);
int dr_verify_restore_point(uint32_t dr_id, uint64_t restore_point_id);
int dr_restore_from_point(uint32_t dr_id, uint64_t restore_point_id);
int dr_delete_restore_point(uint32_t dr_id, uint64_t restore_point_id);
int dr_get_restore_points(uint32_t dr_id, RestorePoint *points, uint32_t *count);

int dr_add_replicated_system(uint32_t dr_id, const char *system_name,
                             const char *primary_addr, const char *secondary_addr);
int dr_remove_replicated_system(uint32_t dr_id, uint32_t system_id);
int dr_start_replication(uint32_t dr_id, uint32_t system_id);
int dr_stop_replication(uint32_t dr_id, uint32_t system_id);
int dr_update_system_status(uint32_t dr_id, uint32_t system_id, SystemStatus status);
int dr_get_replication_lag(uint32_t dr_id, uint32_t system_id, uint64_t *lag_bytes);

int dr_add_failover_config(uint32_t dr_id, const char *failover_name, FailoverType failover_type,
                           uint32_t system_id, uint32_t rto_sec, uint32_t rpo_sec);
int dr_remove_failover_config(uint32_t dr_id, uint32_t failover_id);
int dr_execute_failover(uint32_t dr_id, uint32_t failover_id);
int dr_execute_failback(uint32_t dr_id, uint32_t failover_id);
int dr_test_failover(uint32_t dr_id, uint32_t failover_id);

int dr_add_recovery_plan(uint32_t dr_id, const char *plan_name);
int dr_remove_recovery_plan(uint32_t dr_id, uint32_t plan_id);
int dr_add_system_to_plan(uint32_t dr_id, uint32_t plan_id, uint32_t system_id);
int dr_execute_recovery_plan(uint32_t dr_id, uint32_t plan_id);
int dr_test_recovery_plan(uint32_t dr_id, uint32_t plan_id);

uint64_t dr_get_total_backups(uint32_t dr_id);
uint64_t dr_get_total_restores(uint32_t dr_id);
uint64_t dr_get_total_failovers(uint32_t dr_id);
uint64_t dr_get_backup_storage_used(uint32_id dr_id);

int dr_export_dr_config(uint32_t dr_id, const char *format);
int dr_generate_dr_report(uint32_t dr_id, const char *output_path);

DisasterRecoveryManager *dr_get_manager_info(uint32_t dr_id);

#endif
