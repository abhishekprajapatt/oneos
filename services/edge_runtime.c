#include "edge_runtime.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static EdgeRuntime g_runtimes[MAX_EDGE_RUNTIMES] = {0};
static uint32_t g_runtime_count = 0;
static uint64_t g_task_id_counter = 1;
static uint64_t g_metric_id_counter = 1;

int edge_create_runtime(const char *device_name, DeviceType device_type,
                        uint32_t cpu_cores, uint64_t memory_bytes, uint64_t storage_bytes)
{
    if (!device_name || g_runtime_count >= MAX_EDGE_RUNTIMES)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[g_runtime_count];
    runtime->runtime_id = g_runtime_count + 1;
    strncpy(runtime->device_name, device_name, MAX_DEVICE_NAME - 1);
    runtime->device_type = device_type;

    runtime->container_count = 0;
    runtime->deployment_count = 0;
    runtime->task_count = 0;
    runtime->metric_count = 0;

    runtime->total_capacity.cpu_cores = cpu_cores;
    runtime->total_capacity.memory_bytes = memory_bytes;
    runtime->total_capacity.storage_bytes = storage_bytes;
    runtime->total_capacity.network_mbps = 1000;

    runtime->available_resources = runtime->total_capacity;

    runtime->is_connected = 1;
    runtime->last_heartbeat = 0;
    runtime->health_score = 100.0f;

    runtime->total_tasks_executed = 0;
    runtime->total_tasks_failed = 0;
    runtime->total_bytes_processed = 0;

    g_runtime_count++;
    return runtime->runtime_id;
}

int edge_delete_runtime(uint32_t runtime_id)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count)
    {
        return -1;
    }

    for (uint32_t i = runtime_id - 1; i < g_runtime_count - 1; i++)
    {
        g_runtimes[i] = g_runtimes[i + 1];
    }
    g_runtime_count--;

    return 0;
}

int edge_create_container(uint32_t runtime_id, const char *name, const char *image_hash)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count || !name || !image_hash)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];
    if (runtime->container_count >= MAX_EDGE_CONTAINERS)
    {
        return -1;
    }

    EdgeContainer *container = &runtime->containers[runtime->container_count];
    container->container_id = runtime->container_count + 1;
    strncpy(container->name, name, MAX_DEVICE_NAME - 1);
    strncpy(container->image_hash, image_hash, 63);

    container->state = CONTAINER_IDLE;
    container->created_at = 0;
    container->started_at = 0;
    container->last_activity = 0;

    container->cpu_used = 0;
    container->memory_used = 0;
    container->storage_used = 0;
    container->network_used = 0;

    container->limits.cpu_cores = 1;
    container->limits.memory_bytes = 512 * 1024 * 1024;
    container->limits.storage_bytes = 2 * 1024 * 1024 * 1024;
    container->limits.network_mbps = 100;

    container->is_privileged = 0;
    container->network_enabled = 1;

    runtime->container_count++;

    return container->container_id;
}

int edge_delete_container(uint32_t runtime_id, uint32_t container_id)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];

    for (uint32_t i = 0; i < runtime->container_count; i++)
    {
        if (runtime->containers[i].container_id == container_id)
        {
            for (uint32_t j = i; j < runtime->container_count - 1; j++)
            {
                runtime->containers[j] = runtime->containers[j + 1];
            }
            runtime->container_count--;
            return 0;
        }
    }

    return -1;
}

int edge_start_container(uint32_t runtime_id, uint32_t container_id)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];

    for (uint32_t i = 0; i < runtime->container_count; i++)
    {
        if (runtime->containers[i].container_id == container_id)
        {
            runtime->containers[i].state = CONTAINER_RUNNING;
            runtime->containers[i].started_at = 0;
            runtime->containers[i].last_activity = 0;

            runtime->available_resources.cpu_cores -= runtime->containers[i].limits.cpu_cores;
            runtime->available_resources.memory_bytes -= runtime->containers[i].limits.memory_bytes;

            return 0;
        }
    }

    return -1;
}

int edge_stop_container(uint32_t runtime_id, uint32_t container_id)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];

    for (uint32_t i = 0; i < runtime->container_count; i++)
    {
        if (runtime->containers[i].container_id == container_id)
        {
            EdgeContainer *container = &runtime->containers[i];

            runtime->available_resources.cpu_cores += container->limits.cpu_cores;
            runtime->available_resources.memory_bytes += container->limits.memory_bytes;

            container->state = CONTAINER_STOPPED;
            return 0;
        }
    }

    return -1;
}

int edge_pause_container(uint32_t runtime_id, uint32_t container_id)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];

    for (uint32_t i = 0; i < runtime->container_count; i++)
    {
        if (runtime->containers[i].container_id == container_id)
        {
            runtime->containers[i].state = CONTAINER_PAUSED;
            return 0;
        }
    }

    return -1;
}

int edge_resume_container(uint32_t runtime_id, uint32_t container_id)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];

    for (uint32_t i = 0; i < runtime->container_count; i++)
    {
        if (runtime->containers[i].container_id == container_id)
        {
            runtime->containers[i].state = CONTAINER_RUNNING;
            return 0;
        }
    }

    return -1;
}

int edge_deploy_application(uint32_t runtime_id, const char *app_name, const char *version,
                            uint32_t replicas, ResourceLimit *resources)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count || !app_name || !version || !resources)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];
    if (runtime->deployment_count >= MAX_EDGE_DEPLOYMENTS)
    {
        return -1;
    }

    EdgeDeployment *deployment = &runtime->deployments[runtime->deployment_count];
    deployment->deployment_id = runtime->deployment_count + 1;
    snprintf(deployment->name, MAX_DEPLOYMENT_NAME, "%s-%s", app_name, version);
    strncpy(deployment->app_name, app_name, 127);
    strncpy(deployment->version, version, 31);

    deployment->replicas = replicas;
    deployment->running_replicas = 0;

    deployment->min_resources = *resources;
    deployment->max_resources.cpu_cores = resources->cpu_cores * 2;
    deployment->max_resources.memory_bytes = resources->memory_bytes * 2;
    deployment->max_resources.storage_bytes = resources->storage_bytes;
    deployment->max_resources.network_mbps = resources->network_mbps;

    deployment->auto_scaling_enabled = 1;
    deployment->scale_up_threshold = 80.0f;
    deployment->scale_down_threshold = 30.0f;

    deployment->deployed_at = 0;
    deployment->last_updated = 0;
    deployment->is_active = 1;

    runtime->deployment_count++;

    return deployment->deployment_id;
}

int edge_undeploy_application(uint32_t runtime_id, uint32_t deployment_id)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];

    for (uint32_t i = 0; i < runtime->deployment_count; i++)
    {
        if (runtime->deployments[i].deployment_id == deployment_id)
        {
            runtime->deployments[i].is_active = 0;
            return 0;
        }
    }

    return -1;
}

int edge_update_deployment(uint32_t runtime_id, uint32_t deployment_id, uint32_t new_replicas)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];

    for (uint32_t i = 0; i < runtime->deployment_count; i++)
    {
        if (runtime->deployments[i].deployment_id == deployment_id)
        {
            runtime->deployments[i].replicas = new_replicas;
            runtime->deployments[i].last_updated = 0;
            return 0;
        }
    }

    return -1;
}

int edge_set_auto_scaling(uint32_t runtime_id, uint32_t deployment_id, float up_threshold, float down_threshold)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];

    for (uint32_t i = 0; i < runtime->deployment_count; i++)
    {
        if (runtime->deployments[i].deployment_id == deployment_id)
        {
            runtime->deployments[i].auto_scaling_enabled = 1;
            runtime->deployments[i].scale_up_threshold = up_threshold;
            runtime->deployments[i].scale_down_threshold = down_threshold;
            return 0;
        }
    }

    return -1;
}

int edge_submit_task(uint32_t runtime_id, uint32_t deployment_id, const char *task_name,
                     TaskPriority priority, uint64_t timeout_ms)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count || !task_name)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];
    if (runtime->task_count >= MAX_EDGE_TASKS)
    {
        return -1;
    }

    EdgeTask *task = &runtime->tasks[runtime->task_count];
    task->task_id = g_task_id_counter++;
    task->deployment_id = deployment_id;
    strncpy(task->task_name, task_name, 63);
    task->priority = priority;
    task->state = TASK_QUEUED;

    task->submitted_at = 0;
    task->started_at = 0;
    task->completed_at = 0;
    task->timeout_ms = timeout_ms;

    task->cpu_cores = 1;
    task->memory_bytes = 256 * 1024 * 1024;
    task->retry_count = 0;
    task->max_retries = 3;

    runtime->task_count++;

    return task->task_id;
}

int edge_get_task_status(uint32_t runtime_id, uint64_t task_id, TaskState *state)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count || !state)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];

    for (uint32_t i = 0; i < runtime->task_count; i++)
    {
        if (runtime->tasks[i].task_id == task_id)
        {
            *state = runtime->tasks[i].state;
            return 0;
        }
    }

    return -1;
}

int edge_cancel_task(uint32_t runtime_id, uint64_t task_id)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];

    for (uint32_t i = 0; i < runtime->task_count; i++)
    {
        if (runtime->tasks[i].task_id == task_id)
        {
            if (runtime->tasks[i].state == TASK_RUNNING || runtime->tasks[i].state == TASK_QUEUED)
            {
                runtime->tasks[i].state = TASK_FAILED;
                return 0;
            }
        }
    }

    return -1;
}

int edge_get_task_result(uint32_t runtime_id, uint64_t task_id, char *result, uint32_t result_len)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count || !result)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];

    for (uint32_t i = 0; i < runtime->task_count; i++)
    {
        if (runtime->tasks[i].task_id == task_id)
        {
            uint32_t copy_len = (strlen(runtime->tasks[i].result) < result_len) ? strlen(runtime->tasks[i].result) : result_len - 1;
            strncpy(result, runtime->tasks[i].result, copy_len);
            result[copy_len] = '\0';
            return 0;
        }
    }

    return -1;
}

int edge_record_metric(uint32_t runtime_id, uint32_t container_id, const char *metric_name, float value)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count || !metric_name)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];
    if (runtime->metric_count >= (MAX_EDGE_METRICS * 32))
    {
        return -1;
    }

    EdgeMetric *metric = &runtime->metrics[runtime->metric_count];
    metric->metric_id = g_metric_id_counter++;
    strncpy(metric->metric_name, metric_name, 63);
    metric->value = value;
    metric->timestamp = 0;
    metric->container_id = container_id;

    runtime->metric_count++;

    return 0;
}

int edge_get_metrics(uint32_t runtime_id, uint32_t container_id, EdgeMetric *metrics, uint32_t *count)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count || !metrics || !count)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];
    uint32_t metric_count = 0;

    for (uint32_t i = 0; i < runtime->metric_count && metric_count < *count; i++)
    {
        if (container_id == 0 || runtime->metrics[i].container_id == container_id)
        {
            metrics[metric_count++] = runtime->metrics[i];
        }
    }

    *count = metric_count;
    return 0;
}

int edge_get_container_resource_usage(uint32_t runtime_id, uint32_t container_id, ResourceLimit *usage)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count || !usage)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];

    for (uint32_t i = 0; i < runtime->container_count; i++)
    {
        if (runtime->containers[i].container_id == container_id)
        {
            usage->cpu_cores = runtime->containers[i].cpu_used;
            usage->memory_bytes = runtime->containers[i].memory_used;
            usage->storage_bytes = runtime->containers[i].storage_used;
            usage->network_mbps = runtime->containers[i].network_used;
            return 0;
        }
    }

    return -1;
}

int edge_allocate_resources(uint32_t runtime_id, uint32_t container_id, ResourceLimit *limit)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count || !limit)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];

    for (uint32_t i = 0; i < runtime->container_count; i++)
    {
        if (runtime->containers[i].container_id == container_id)
        {
            runtime->containers[i].limits = *limit;
            return 0;
        }
    }

    return -1;
}

int edge_deallocate_resources(uint32_t runtime_id, uint32_t container_id)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];

    for (uint32_t i = 0; i < runtime->container_count; i++)
    {
        if (runtime->containers[i].container_id == container_id)
        {
            memset(&runtime->containers[i].limits, 0, sizeof(ResourceLimit));
            return 0;
        }
    }

    return -1;
}

int edge_migrate_container(uint32_t src_runtime_id, uint32_t dst_runtime_id, uint32_t container_id)
{
    if (src_runtime_id == 0 || src_runtime_id > g_runtime_count ||
        dst_runtime_id == 0 || dst_runtime_id > g_runtime_count)
    {
        return -1;
    }

    return 0;
}

int edge_set_connected(uint32_t runtime_id, uint32_t is_connected)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count)
    {
        return -1;
    }

    g_runtimes[runtime_id - 1].is_connected = is_connected;
    return 0;
}

int edge_update_health_score(uint32_t runtime_id, float health)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count)
    {
        return -1;
    }

    if (health < 0.0f)
        health = 0.0f;
    if (health > 100.0f)
        health = 100.0f;

    g_runtimes[runtime_id - 1].health_score = health;
    return 0;
}

int edge_get_runtime_stats(uint32_t runtime_id, uint64_t *tasks_executed, uint64_t *tasks_failed, uint64_t *bytes_processed)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count || !tasks_executed || !tasks_failed || !bytes_processed)
    {
        return -1;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];
    *tasks_executed = runtime->total_tasks_executed;
    *tasks_failed = runtime->total_tasks_failed;
    *bytes_processed = runtime->total_bytes_processed;

    return 0;
}

EdgeRuntime *edge_get_runtime_info(uint32_t runtime_id)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count)
    {
        return NULL;
    }

    return &g_runtimes[runtime_id - 1];
}

EdgeContainer *edge_get_container_info(uint32_t runtime_id, uint32_t container_id)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count)
    {
        return NULL;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];

    for (uint32_t i = 0; i < runtime->container_count; i++)
    {
        if (runtime->containers[i].container_id == container_id)
        {
            return &runtime->containers[i];
        }
    }

    return NULL;
}

EdgeDeployment *edge_get_deployment_info(uint32_t runtime_id, uint32_t deployment_id)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count)
    {
        return NULL;
    }

    EdgeRuntime *runtime = &g_runtimes[runtime_id - 1];

    for (uint32_t i = 0; i < runtime->deployment_count; i++)
    {
        if (runtime->deployments[i].deployment_id == deployment_id)
        {
            return &runtime->deployments[i];
        }
    }

    return NULL;
}

int edge_export_runtime_state(uint32_t runtime_id, const char *format)
{
    if (runtime_id == 0 || runtime_id > g_runtime_count || !format)
    {
        return -1;
    }

    return 0;
}
