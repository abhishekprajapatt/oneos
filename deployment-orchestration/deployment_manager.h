#ifndef DEPLOYMENT_MANAGER_H
#define DEPLOYMENT_MANAGER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_DEPLOYMENTS 512
#define MAX_DEPLOYMENT_STAGES 16
#define MAX_DEPLOYMENT_TASKS 1024
#define MAX_DEPLOYMENT_LOGS 4096

typedef enum
{
    DEPLOYMENT_STATUS_PENDING,
    DEPLOYMENT_STATUS_IN_PROGRESS,
    DEPLOYMENT_STATUS_COMPLETED,
    DEPLOYMENT_STATUS_FAILED,
    DEPLOYMENT_STATUS_ROLLED_BACK
} DeploymentStatus;

typedef enum
{
    STAGE_TYPE_BUILD,
    STAGE_TYPE_TEST,
    STAGE_TYPE_STAGING,
    STAGE_TYPE_CANARY,
    STAGE_TYPE_PRODUCTION,
    STAGE_TYPE_ROLLBACK
} StageType;

typedef enum
{
    TASK_STATUS_PENDING,
    TASK_STATUS_RUNNING,
    TASK_STATUS_COMPLETED,
    TASK_STATUS_FAILED
} TaskStatus;

typedef struct
{
    uint32_t stage_id;
    StageType stage_type;
    char stage_name[64];

    uint32_t task_ids[MAX_DEPLOYMENT_TASKS];
    uint32_t task_count;

    uint64_t stage_start_time;
    uint64_t stage_end_time;
    uint8_t is_complete;
} DeploymentStage;

typedef struct
{
    uint32_t task_id;
    char task_name[128];
    char task_command[512];

    TaskStatus status;
    uint64_t task_start_time;
    uint64_t task_end_time;
    int32_t task_return_code;
} DeploymentTask;

typedef struct
{
    uint32_t deployment_id;
    char deployment_name[128];
    char application_version[64];

    DeploymentStage stages[MAX_DEPLOYMENT_STAGES];
    uint32_t stage_count;

    DeploymentTask tasks[MAX_DEPLOYMENT_TASKS];
    uint32_t task_count;

    DeploymentStatus status;
    uint8_t is_rollback_enabled;

    uint64_t deployment_start_time;
    uint64_t deployment_end_time;
    uint32_t deployment_duration_ms;
} Deployment;

typedef struct
{
    uint32_t manager_id;
    Deployment deployments[MAX_DEPLOYMENTS];
    uint32_t deployment_count;

    uint32_t active_deployments;
    uint64_t total_deployments_created;
    uint32_t successful_deployments;
    uint32_t failed_deployments;
    uint32_t rolled_back_deployments;
} DeploymentManager;

DeploymentManager *deployment_manager_init(void);
int deployment_manager_destroy(DeploymentManager *manager);

uint32_t deployment_create(DeploymentManager *manager, const char *deployment_name,
                           const char *application_version);
int deployment_destroy(DeploymentManager *manager, uint32_t deployment_id);

int deployment_add_stage(DeploymentManager *manager, uint32_t deployment_id,
                         StageType stage_type, const char *stage_name);

int deployment_add_task(DeploymentManager *manager, uint32_t deployment_id,
                        uint32_t stage_id, const char *task_name,
                        const char *task_command);

int deployment_start(DeploymentManager *manager, uint32_t deployment_id);
int deployment_execute_stage(DeploymentManager *manager, uint32_t deployment_id,
                             uint32_t stage_id);

int deployment_execute_task(DeploymentManager *manager, uint32_t deployment_id,
                            uint32_t task_id);

int deployment_complete_stage(DeploymentManager *manager, uint32_t deployment_id,
                              uint32_t stage_id);

int deployment_mark_task_complete(DeploymentManager *manager, uint32_t deployment_id,
                                  uint32_t task_id, int32_t return_code);

int deployment_rollback(DeploymentManager *manager, uint32_t deployment_id);
int deployment_enable_rollback(DeploymentManager *manager, uint32_t deployment_id);

int deployment_get_status(DeploymentManager *manager, uint32_t deployment_id,
                          DeploymentStatus *status);

int deployment_get_current_stage(DeploymentManager *manager, uint32_t deployment_id,
                                 uint32_t *stage_id);

typedef struct
{
    uint32_t manager_id;
    uint32_t total_deployments;
    uint32_t active_deployments;
    uint32_t successful_deployments;
    uint32_t failed_deployments;
    uint32_t rolled_back_deployments;
    float success_rate;
} DeploymentManagerMetrics;

int deployment_manager_get_metrics(DeploymentManager *manager, DeploymentManagerMetrics *metrics);

#endif
