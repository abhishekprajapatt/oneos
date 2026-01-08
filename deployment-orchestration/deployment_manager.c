#include "deployment_manager.h"
#include <stdlib.h>
#include <string.h>

static DeploymentManager *global_deployment_manager = NULL;
static uint32_t next_deployment_id = 1;
static uint32_t next_stage_id = 1;
static uint32_t next_task_id = 1;

DeploymentManager *deployment_manager_init(void)
{
    DeploymentManager *manager = (DeploymentManager *)malloc(sizeof(DeploymentManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(DeploymentManager));
    manager->manager_id = 1;
    manager->deployment_count = 0;
    manager->active_deployments = 0;
    manager->total_deployments_created = 0;
    manager->successful_deployments = 0;
    manager->failed_deployments = 0;
    manager->rolled_back_deployments = 0;

    global_deployment_manager = manager;
    return manager;
}

int deployment_manager_destroy(DeploymentManager *manager)
{
    if (!manager)
        return -1;

    free(manager);
    global_deployment_manager = NULL;
    return 0;
}

uint32_t deployment_create(DeploymentManager *manager, const char *deployment_name,
                           const char *application_version)
{
    if (!manager || manager->deployment_count >= MAX_DEPLOYMENTS || !deployment_name)
        return 0;

    Deployment *deployment = &manager->deployments[manager->deployment_count];
    memset(deployment, 0, sizeof(Deployment));

    deployment->deployment_id = next_deployment_id++;
    strncpy(deployment->deployment_name, deployment_name, sizeof(deployment->deployment_name) - 1);

    if (application_version)
    {
        strncpy(deployment->application_version, application_version, sizeof(deployment->application_version) - 1);
    }

    deployment->stage_count = 0;
    deployment->task_count = 0;
    deployment->status = DEPLOYMENT_STATUS_PENDING;
    deployment->is_rollback_enabled = 0;
    deployment->deployment_start_time = 0;
    deployment->deployment_end_time = 0;
    deployment->deployment_duration_ms = 0;

    manager->deployment_count++;
    manager->total_deployments_created++;

    return deployment->deployment_id;
}

int deployment_destroy(DeploymentManager *manager, uint32_t deployment_id)
{
    if (!manager || deployment_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->deployment_count; i++)
    {
        if (manager->deployments[i].deployment_id == deployment_id)
        {
            memmove(&manager->deployments[i], &manager->deployments[i + 1],
                    sizeof(Deployment) * (manager->deployment_count - i - 1));
            manager->deployment_count--;
            return 0;
        }
    }

    return -1;
}

int deployment_add_stage(DeploymentManager *manager, uint32_t deployment_id,
                         StageType stage_type, const char *stage_name)
{
    if (!manager || deployment_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->deployment_count; i++)
    {
        if (manager->deployments[i].deployment_id == deployment_id)
        {
            Deployment *deployment = &manager->deployments[i];

            if (deployment->stage_count >= MAX_DEPLOYMENT_STAGES)
                return -1;

            DeploymentStage *stage = &deployment->stages[deployment->stage_count];
            memset(stage, 0, sizeof(DeploymentStage));

            stage->stage_id = next_stage_id++;
            stage->stage_type = stage_type;

            if (stage_name)
            {
                strncpy(stage->stage_name, stage_name, sizeof(stage->stage_name) - 1);
            }

            stage->task_count = 0;
            stage->stage_start_time = 0;
            stage->stage_end_time = 0;
            stage->is_complete = 0;

            deployment->stage_count++;
            return stage->stage_id;
        }
    }

    return -1;
}

int deployment_add_task(DeploymentManager *manager, uint32_t deployment_id,
                        uint32_t stage_id, const char *task_name,
                        const char *task_command)
{
    if (!manager || deployment_id == 0 || stage_id == 0 || !task_name || !task_command)
        return -1;

    for (uint32_t i = 0; i < manager->deployment_count; i++)
    {
        if (manager->deployments[i].deployment_id == deployment_id)
        {
            Deployment *deployment = &manager->deployments[i];

            if (deployment->task_count >= MAX_DEPLOYMENT_TASKS)
                return -1;

            DeploymentTask *task = &deployment->tasks[deployment->task_count];
            memset(task, 0, sizeof(DeploymentTask));

            task->task_id = next_task_id++;
            strncpy(task->task_name, task_name, sizeof(task->task_name) - 1);
            strncpy(task->task_command, task_command, sizeof(task->task_command) - 1);

            task->status = TASK_STATUS_PENDING;
            task->task_start_time = 0;
            task->task_end_time = 0;
            task->task_return_code = 0;

            deployment->task_count++;

            for (uint32_t j = 0; j < deployment->stage_count; j++)
            {
                if (deployment->stages[j].stage_id == stage_id)
                {
                    if (deployment->stages[j].task_count < MAX_DEPLOYMENT_TASKS)
                    {
                        deployment->stages[j].task_ids[deployment->stages[j].task_count++] = task->task_id;
                    }
                    break;
                }
            }

            return task->task_id;
        }
    }

    return -1;
}

int deployment_start(DeploymentManager *manager, uint32_t deployment_id)
{
    if (!manager || deployment_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->deployment_count; i++)
    {
        if (manager->deployments[i].deployment_id == deployment_id)
        {
            Deployment *deployment = &manager->deployments[i];

            if (deployment->status == DEPLOYMENT_STATUS_PENDING)
            {
                deployment->status = DEPLOYMENT_STATUS_IN_PROGRESS;
                deployment->deployment_start_time = 0;
                manager->active_deployments++;
                return 0;
            }

            return -1;
        }
    }

    return -1;
}

int deployment_execute_stage(DeploymentManager *manager, uint32_t deployment_id,
                             uint32_t stage_id)
{
    if (!manager || deployment_id == 0 || stage_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->deployment_count; i++)
    {
        if (manager->deployments[i].deployment_id == deployment_id)
        {
            Deployment *deployment = &manager->deployments[i];

            for (uint32_t j = 0; j < deployment->stage_count; j++)
            {
                if (deployment->stages[j].stage_id == stage_id)
                {
                    DeploymentStage *stage = &deployment->stages[j];
                    stage->stage_start_time = 0;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int deployment_execute_task(DeploymentManager *manager, uint32_t deployment_id,
                            uint32_t task_id)
{
    if (!manager || deployment_id == 0 || task_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->deployment_count; i++)
    {
        if (manager->deployments[i].deployment_id == deployment_id)
        {
            Deployment *deployment = &manager->deployments[i];

            for (uint32_t j = 0; j < deployment->task_count; j++)
            {
                if (deployment->tasks[j].task_id == task_id)
                {
                    DeploymentTask *task = &deployment->tasks[j];

                    if (task->status == TASK_STATUS_PENDING)
                    {
                        task->status = TASK_STATUS_RUNNING;
                        task->task_start_time = 0;
                        return 0;
                    }
                }
            }
        }
    }

    return -1;
}

int deployment_complete_stage(DeploymentManager *manager, uint32_t deployment_id,
                              uint32_t stage_id)
{
    if (!manager || deployment_id == 0 || stage_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->deployment_count; i++)
    {
        if (manager->deployments[i].deployment_id == deployment_id)
        {
            Deployment *deployment = &manager->deployments[i];

            for (uint32_t j = 0; j < deployment->stage_count; j++)
            {
                if (deployment->stages[j].stage_id == stage_id)
                {
                    DeploymentStage *stage = &deployment->stages[j];
                    stage->stage_end_time = 0;
                    stage->is_complete = 1;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int deployment_mark_task_complete(DeploymentManager *manager, uint32_t deployment_id,
                                  uint32_t task_id, int32_t return_code)
{
    if (!manager || deployment_id == 0 || task_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->deployment_count; i++)
    {
        if (manager->deployments[i].deployment_id == deployment_id)
        {
            Deployment *deployment = &manager->deployments[i];

            for (uint32_t j = 0; j < deployment->task_count; j++)
            {
                if (deployment->tasks[j].task_id == task_id)
                {
                    DeploymentTask *task = &deployment->tasks[j];
                    task->status = TASK_STATUS_COMPLETED;
                    task->task_end_time = 0;
                    task->task_return_code = return_code;

                    if (return_code != 0)
                    {
                        task->status = TASK_STATUS_FAILED;
                        deployment->status = DEPLOYMENT_STATUS_FAILED;
                        manager->failed_deployments++;
                    }

                    return 0;
                }
            }
        }
    }

    return -1;
}

int deployment_rollback(DeploymentManager *manager, uint32_t deployment_id)
{
    if (!manager || deployment_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->deployment_count; i++)
    {
        if (manager->deployments[i].deployment_id == deployment_id)
        {
            Deployment *deployment = &manager->deployments[i];

            if (deployment->is_rollback_enabled)
            {
                deployment->status = DEPLOYMENT_STATUS_ROLLED_BACK;
                manager->rolled_back_deployments++;
                manager->active_deployments--;
                return 0;
            }

            return -1;
        }
    }

    return -1;
}

int deployment_enable_rollback(DeploymentManager *manager, uint32_t deployment_id)
{
    if (!manager || deployment_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->deployment_count; i++)
    {
        if (manager->deployments[i].deployment_id == deployment_id)
        {
            manager->deployments[i].is_rollback_enabled = 1;
            return 0;
        }
    }

    return -1;
}

int deployment_get_status(DeploymentManager *manager, uint32_t deployment_id,
                          DeploymentStatus *status)
{
    if (!manager || deployment_id == 0 || !status)
        return -1;

    for (uint32_t i = 0; i < manager->deployment_count; i++)
    {
        if (manager->deployments[i].deployment_id == deployment_id)
        {
            *status = manager->deployments[i].status;
            return 0;
        }
    }

    return -1;
}

int deployment_get_current_stage(DeploymentManager *manager, uint32_t deployment_id,
                                 uint32_t *stage_id)
{
    if (!manager || deployment_id == 0 || !stage_id)
        return -1;

    for (uint32_t i = 0; i < manager->deployment_count; i++)
    {
        if (manager->deployments[i].deployment_id == deployment_id)
        {
            Deployment *deployment = &manager->deployments[i];

            for (uint32_t j = 0; j < deployment->stage_count; j++)
            {
                if (!deployment->stages[j].is_complete)
                {
                    *stage_id = deployment->stages[j].stage_id;
                    return 0;
                }
            }

            return -1;
        }
    }

    return -1;
}

int deployment_manager_get_metrics(DeploymentManager *manager, DeploymentManagerMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    metrics->manager_id = manager->manager_id;
    metrics->total_deployments = manager->deployment_count;
    metrics->active_deployments = manager->active_deployments;
    metrics->successful_deployments = manager->successful_deployments;
    metrics->failed_deployments = manager->failed_deployments;
    metrics->rolled_back_deployments = manager->rolled_back_deployments;

    if (manager->total_deployments_created > 0)
    {
        metrics->success_rate = (float)manager->successful_deployments / manager->total_deployments_created;
    }
    else
    {
        metrics->success_rate = 0.0f;
    }

    return 0;
}
