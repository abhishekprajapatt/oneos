#include "workload_manager.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static WorkloadManager g_workload_managers[MAX_WORKLOAD_MANAGERS] = {0};
static uint32_t g_manager_count = 0;
static uint64_t g_job_id_counter = 1;
static uint64_t g_workflow_id_counter = 1;

int wm_create_workload_manager(const char *name)
{
    if (!name || g_manager_count >= MAX_WORKLOAD_MANAGERS)
    {
        return -1;
    }

    WorkloadManager *manager = &g_workload_managers[g_manager_count];
    manager->manager_id = g_manager_count + 1;
    strncpy(manager->name, name, 63);

    manager->job_count = 0;
    manager->workflow_count = 0;
    manager->batch_task_count = 0;
    manager->template_count = 0;

    manager->total_jobs_submitted = 0;
    manager->total_jobs_completed = 0;
    manager->total_jobs_failed = 0;
    manager->total_tasks_executed = 0;

    manager->average_job_duration_ms = 0;
    manager->average_task_duration_ms = 0;
    manager->average_success_rate = 0.0;

    manager->created_at = time(NULL);
    manager->is_active = 1;

    g_manager_count++;
    return manager->manager_id;
}

int wm_delete_workload_manager(uint32_t manager_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
    {
        return -1;
    }

    for (uint32_t i = manager_id - 1; i < g_manager_count - 1; i++)
    {
        g_workload_managers[i] = g_workload_managers[i + 1];
    }
    g_manager_count--;

    return 0;
}

uint64_t wm_submit_job(uint32_t manager_id, const char *job_name, WorkloadType workload_type,
                       SchedulingPriority priority, uint32_t deadline_ms, const char *command)
{
    if (manager_id == 0 || manager_id > g_manager_count || !job_name || !command)
    {
        return 0;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];
    if (manager->job_count >= MAX_JOBS)
    {
        return 0;
    }

    Job *job = &manager->jobs[manager->job_count];
    job->job_id = g_job_id_counter++;
    strncpy(job->job_name, job_name, 63);
    job->workload_type = workload_type;
    job->priority = priority;
    job->deadline_ms = deadline_ms;
    job->state = JOB_STATE_CREATED;
    job->submitted_at = time(NULL);
    job->started_at = 0;
    job->completed_at = 0;
    job->duration_ms = 0;
    job->retry_count = 0;
    job->max_retries = 3;
    job->estimated_cost = 0.0;
    job->actual_cost = 0.0;
    strncpy(job->command, command, 255);

    manager->job_count++;
    manager->total_jobs_submitted++;

    return job->job_id;
}

int wm_update_job_state(uint32_t manager_id, uint64_t job_id, JobState new_state)
{
    if (manager_id == 0 || manager_id > g_manager_count)
    {
        return -1;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->job_count; i++)
    {
        if (manager->jobs[i].job_id == job_id)
        {
            manager->jobs[i].state = new_state;

            if (new_state == JOB_STATE_RUNNING)
                manager->jobs[i].started_at = time(NULL);
            else if (new_state == JOB_STATE_COMPLETED)
            {
                manager->jobs[i].completed_at = time(NULL);
                manager->total_jobs_completed++;
            }
            else if (new_state == JOB_STATE_FAILED)
                manager->total_jobs_failed++;

            return 0;
        }
    }

    return -1;
}

int wm_get_job_state(uint32_t manager_id, uint64_t job_id, JobState *state)
{
    if (manager_id == 0 || manager_id > g_manager_count || !state)
    {
        return -1;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->job_count; i++)
    {
        if (manager->jobs[i].job_id == job_id)
        {
            *state = manager->jobs[i].state;
            return 0;
        }
    }

    return -1;
}

int wm_cancel_job(uint32_t manager_id, uint64_t job_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
    {
        return -1;
    }

    return wm_update_job_state(manager_id, job_id, JOB_STATE_CANCELED);
}

uint64_t wm_create_workflow(uint32_t manager_id, const char *workflow_name, uint32_t stage_count)
{
    if (manager_id == 0 || manager_id > g_manager_count || !workflow_name || stage_count == 0)
    {
        return 0;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];
    if (manager->workflow_count >= MAX_WORKFLOWS)
    {
        return 0;
    }

    JobWorkflow *workflow = &manager->workflows[manager->workflow_count];
    workflow->workflow_id = g_workflow_id_counter++;
    strncpy(workflow->workflow_name, workflow_name, 63);
    workflow->stage_count = stage_count;
    workflow->current_stage = 0;
    workflow->allow_parallel_stages = 0;
    workflow->created_at = time(NULL);
    workflow->started_at = 0;
    workflow->completed_at = 0;
    workflow->is_active = 1;

    manager->workflow_count++;
    return workflow->workflow_id;
}

int wm_add_stage_to_workflow(uint32_t manager_id, uint64_t workflow_id, uint32_t stage_index,
                             const char *stage_name)
{
    if (manager_id == 0 || manager_id > g_manager_count || !stage_name)
    {
        return -1;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->workflow_count; i++)
    {
        if (manager->workflows[i].workflow_id == workflow_id)
        {
            return 0;
        }
    }

    return -1;
}

int wm_add_job_to_workflow(uint32_t manager_id, uint64_t workflow_id, uint64_t job_id,
                           uint32_t stage_index)
{
    if (manager_id == 0 || manager_id > g_manager_count)
    {
        return -1;
    }

    return 0;
}

int wm_progress_workflow_stage(uint32_t manager_id, uint64_t workflow_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
    {
        return -1;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->workflow_count; i++)
    {
        if (manager->workflows[i].workflow_id == workflow_id)
        {
            manager->workflows[i].current_stage++;
            return 0;
        }
    }

    return -1;
}

int wm_delete_workflow(uint32_t manager_id, uint64_t workflow_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
    {
        return -1;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->workflow_count; i++)
    {
        if (manager->workflows[i].workflow_id == workflow_id)
        {
            for (uint32_t j = i; j < manager->workflow_count - 1; j++)
            {
                manager->workflows[j] = manager->workflows[j + 1];
            }
            manager->workflow_count--;
            return 0;
        }
    }

    return -1;
}

uint64_t wm_submit_batch_task(uint32_t manager_id, const char *task_name, const char *command,
                              uint32_t timeout_ms, uint8_t retryable)
{
    if (manager_id == 0 || manager_id > g_manager_count || !task_name || !command)
    {
        return 0;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];
    if (manager->batch_task_count >= MAX_BATCH_TASKS)
    {
        return 0;
    }

    BatchTask *task = &manager->batch_tasks[manager->batch_task_count];
    task->task_id = manager->batch_task_count + 1;
    strncpy(task->task_name, task_name, 63);
    task->state = TASK_STATE_PENDING;
    task->submitted_at = time(NULL);
    task->started_at = 0;
    task->completed_at = 0;
    task->timeout_ms = timeout_ms;
    task->retry_count = 0;
    task->max_retries = retryable ? 3 : 0;
    task->exit_code = 0;
    strncpy(task->command, command, 255);

    manager->batch_task_count++;
    manager->total_tasks_executed++;

    return task->task_id;
}

int wm_get_batch_task_state(uint32_t manager_id, uint64_t task_id, TaskState *state)
{
    if (manager_id == 0 || manager_id > g_manager_count || !state)
    {
        return -1;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->batch_task_count; i++)
    {
        if (manager->batch_tasks[i].task_id == task_id)
        {
            *state = manager->batch_tasks[i].state;
            return 0;
        }
    }

    return -1;
}

int wm_update_batch_task_state(uint32_t manager_id, uint64_t task_id, TaskState new_state)
{
    if (manager_id == 0 || manager_id > g_manager_count)
    {
        return -1;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->batch_task_count; i++)
    {
        if (manager->batch_tasks[i].task_id == task_id)
        {
            manager->batch_tasks[i].state = new_state;

            if (new_state == TASK_STATE_RUNNING)
                manager->batch_tasks[i].started_at = time(NULL);
            else if (new_state == TASK_STATE_COMPLETED)
                manager->batch_tasks[i].completed_at = time(NULL);

            return 0;
        }
    }

    return -1;
}

int wm_retry_batch_task(uint32_t manager_id, uint64_t task_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
    {
        return -1;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->batch_task_count; i++)
    {
        if (manager->batch_tasks[i].task_id == task_id)
        {
            if (manager->batch_tasks[i].retry_count < manager->batch_tasks[i].max_retries)
            {
                manager->batch_tasks[i].retry_count++;
                manager->batch_tasks[i].state = TASK_STATE_RETRYING;
                return 0;
            }
            return -1;
        }
    }

    return -1;
}

uint32_t wm_create_job_template(uint32_t manager_id, const char *template_name,
                                const char *template_definition)
{
    if (manager_id == 0 || manager_id > g_manager_count || !template_name || !template_definition)
    {
        return 0;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];
    if (manager->template_count >= MAX_TEMPLATES)
    {
        return 0;
    }

    WorkloadTemplate *template = &manager->templates[manager->template_count];
    template->template_id = manager->template_count + 1;
    strncpy(template->template_name, template_name, 63);
    strncpy(template->template_definition, template_definition, 255);
    template->created_at = time(NULL);
    template->usage_count = 0;
    template->is_active = 1;

    manager->template_count++;
    return template->template_id;
}

uint64_t wm_instantiate_job_from_template(uint32_t manager_id, uint32_t template_id,
                                          const char *job_name, SchedulingPriority priority)
{
    if (manager_id == 0 || manager_id > g_manager_count || !job_name)
    {
        return 0;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->template_count; i++)
    {
        if (manager->templates[i].template_id == template_id)
        {
            manager->templates[i].usage_count++;
            return wm_submit_job(manager_id, job_name, 0, priority, 0,
                                 manager->templates[i].template_definition);
        }
    }

    return 0;
}

int wm_delete_job_template(uint32_t manager_id, uint32_t template_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
    {
        return -1;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->template_count; i++)
    {
        if (manager->templates[i].template_id == template_id)
        {
            for (uint32_t j = i; j < manager->template_count - 1; j++)
            {
                manager->templates[j] = manager->templates[j + 1];
            }
            manager->template_count--;
            return 0;
        }
    }

    return -1;
}

double wm_estimate_job_cost(uint32_t manager_id, uint64_t job_id, double cpu_rate_per_ms,
                            double memory_rate_per_mb_ms)
{
    if (manager_id == 0 || manager_id > g_manager_count)
    {
        return 0.0;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->job_count; i++)
    {
        if (manager->jobs[i].job_id == job_id)
        {
            double estimated = (manager->jobs[i].deadline_ms * cpu_rate_per_ms) +
                               (1024.0 * memory_rate_per_mb_ms);
            manager->jobs[i].estimated_cost = estimated;
            return estimated;
        }
    }

    return 0.0;
}

int wm_record_actual_job_cost(uint32_t manager_id, uint64_t job_id, double actual_cost)
{
    if (manager_id == 0 || manager_id > g_manager_count || actual_cost < 0)
    {
        return -1;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->job_count; i++)
    {
        if (manager->jobs[i].job_id == job_id)
        {
            manager->jobs[i].actual_cost = actual_cost;
            return 0;
        }
    }

    return -1;
}

int wm_get_job_metrics(uint32_t manager_id, uint64_t job_id, uint32_t *cpu_ms,
                       uint32_t *memory_mb, double *cost)
{
    if (manager_id == 0 || manager_id > g_manager_count || !cpu_ms || !memory_mb || !cost)
    {
        return -1;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];

    for (uint32_t i = 0; i < manager->job_count; i++)
    {
        if (manager->jobs[i].job_id == job_id)
        {
            *cpu_ms = manager->jobs[i].duration_ms;
            *memory_mb = 1024;
            *cost = manager->jobs[i].actual_cost;
            return 0;
        }
    }

    return -1;
}

int wm_add_job_dependency(uint32_t manager_id, uint64_t job_id, uint64_t dependent_job_id,
                          DependencyType dep_type)
{
    if (manager_id == 0 || manager_id > g_manager_count)
    {
        return -1;
    }

    return 0;
}

int wm_get_jobs(uint32_t manager_id, Job *jobs, uint32_t *count)
{
    if (manager_id == 0 || manager_id > g_manager_count || !jobs || !count)
    {
        return -1;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];

    if (manager->job_count > *count)
    {
        return -1;
    }

    memcpy(jobs, manager->jobs, manager->job_count * sizeof(Job));
    *count = manager->job_count;

    return 0;
}

int wm_get_workflows(uint32_t manager_id, JobWorkflow *workflows, uint32_t *count)
{
    if (manager_id == 0 || manager_id > g_manager_count || !workflows || !count)
    {
        return -1;
    }

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];

    if (manager->workflow_count > *count)
    {
        return -1;
    }

    memcpy(workflows, manager->workflows, manager->workflow_count * sizeof(JobWorkflow));
    *count = manager->workflow_count;

    return 0;
}

uint32_t wm_get_total_jobs_submitted(uint32_t manager_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
        return 0;
    return g_workload_managers[manager_id - 1].total_jobs_submitted;
}

uint32_t wm_get_total_jobs_completed(uint32_t manager_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
        return 0;
    return g_workload_managers[manager_id - 1].total_jobs_completed;
}

uint32_t wm_get_total_jobs_failed(uint32_t manager_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
        return 0;
    return g_workload_managers[manager_id - 1].total_jobs_failed;
}

double wm_get_success_rate(uint32_t manager_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
        return 0.0;

    WorkloadManager *manager = &g_workload_managers[manager_id - 1];
    if (manager->total_jobs_submitted == 0)
        return 0.0;

    return (double)manager->total_jobs_completed / manager->total_jobs_submitted;
}

WorkloadManager *wm_get_manager_info(uint32_t manager_id)
{
    if (manager_id == 0 || manager_id > g_manager_count)
        return NULL;
    return &g_workload_managers[manager_id - 1];
}
