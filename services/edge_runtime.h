#ifndef EDGE_RUNTIME_H
#define EDGE_RUNTIME_H

#include <stdint.h>
#include <stddef.h>

#define MAX_EDGE_RUNTIMES 8
#define MAX_EDGE_CONTAINERS 512
#define MAX_EDGE_DEPLOYMENTS 256
#define MAX_EDGE_TASKS 1024
#define MAX_RESOURCE_LIMITS 4
#define MAX_EDGE_METRICS 16
#define MAX_DEVICE_NAME 64
#define MAX_DEPLOYMENT_NAME 128

typedef enum
{
    DEVICE_EDGE_SERVER = 1,
    DEVICE_IOT_GATEWAY = 2,
    DEVICE_SENSOR_NODE = 3,
    DEVICE_MOBILE = 4,
    DEVICE_EMBEDDED = 5
} DeviceType;

typedef enum
{
    CONTAINER_IDLE = 1,
    CONTAINER_RUNNING = 2,
    CONTAINER_PAUSED = 3,
    CONTAINER_STOPPED = 4,
    CONTAINER_ERROR = 5
} ContainerState;

typedef enum
{
    TASK_QUEUED = 1,
    TASK_RUNNING = 2,
    TASK_COMPLETED = 3,
    TASK_FAILED = 4,
    TASK_TIMEOUT = 5
} TaskState;

typedef enum
{
    PRIORITY_LOW = 1,
    PRIORITY_MEDIUM = 2,
    PRIORITY_HIGH = 3,
    PRIORITY_CRITICAL = 4
} TaskPriority;

typedef struct
{
    uint32_t cpu_cores;
    uint64_t memory_bytes;
    uint64_t storage_bytes;
    uint32_t network_mbps;
} ResourceLimit;

typedef struct
{
    uint32_t container_id;
    char name[MAX_DEVICE_NAME];
    DeviceType device_type;

    ResourceLimit limits;
    uint32_t cpu_used;
    uint64_t memory_used;
    uint64_t storage_used;
    uint32_t network_used;

    ContainerState state;
    uint64_t created_at;
    uint64_t started_at;
    uint64_t last_activity;

    char image_hash[64];
    uint32_t is_privileged;
    uint32_t network_enabled;
} EdgeContainer;

typedef struct
{
    uint32_t deployment_id;
    char name[MAX_DEPLOYMENT_NAME];
    char app_name[128];
    char version[32];

    uint32_t replicas;
    uint32_t running_replicas;

    ResourceLimit min_resources;
    ResourceLimit max_resources;

    uint32_t auto_scaling_enabled;
    float scale_up_threshold;
    float scale_down_threshold;

    uint64_t deployed_at;
    uint64_t last_updated;
    uint32_t is_active;
} EdgeDeployment;

typedef struct
{
    uint64_t task_id;
    uint32_t deployment_id;
    char task_name[64];
    TaskPriority priority;
    TaskState state;

    uint64_t submitted_at;
    uint64_t started_at;
    uint64_t completed_at;
    uint64_t timeout_ms;

    uint32_t cpu_cores;
    uint64_t memory_bytes;

    uint32_t retry_count;
    uint32_t max_retries;
    char result[256];
} EdgeTask;

typedef struct
{
    uint64_t metric_id;
    char metric_name[64];
    float value;
    uint64_t timestamp;
    uint32_t container_id;
} EdgeMetric;

typedef struct
{
    uint32_t runtime_id;
    char device_name[MAX_DEVICE_NAME];
    DeviceType device_type;

    EdgeContainer containers[MAX_EDGE_CONTAINERS];
    uint32_t container_count;

    EdgeDeployment deployments[MAX_EDGE_DEPLOYMENTS];
    uint32_t deployment_count;

    EdgeTask tasks[MAX_EDGE_TASKS];
    uint32_t task_count;

    EdgeMetric metrics[MAX_EDGE_METRICS * 32];
    uint32_t metric_count;

    ResourceLimit total_capacity;
    ResourceLimit available_resources;

    uint32_t is_connected;
    uint64_t last_heartbeat;
    float health_score;

    uint64_t total_tasks_executed;
    uint64_t total_tasks_failed;
    uint64_t total_bytes_processed;
} EdgeRuntime;

int edge_create_runtime(const char *device_name, DeviceType device_type,
                        uint32_t cpu_cores, uint64_t memory_bytes, uint64_t storage_bytes);
int edge_delete_runtime(uint32_t runtime_id);

int edge_create_container(uint32_t runtime_id, const char *name, const char *image_hash);
int edge_delete_container(uint32_t runtime_id, uint32_t container_id);
int edge_start_container(uint32_t runtime_id, uint32_t container_id);
int edge_stop_container(uint32_t runtime_id, uint32_t container_id);
int edge_pause_container(uint32_t runtime_id, uint32_t container_id);
int edge_resume_container(uint32_t runtime_id, uint32_t container_id);

int edge_deploy_application(uint32_t runtime_id, const char *app_name, const char *version,
                            uint32_t replicas, ResourceLimit *resources);
int edge_undeploy_application(uint32_t runtime_id, uint32_t deployment_id);
int edge_update_deployment(uint32_t runtime_id, uint32_t deployment_id, uint32_t new_replicas);
int edge_set_auto_scaling(uint32_t runtime_id, uint32_t deployment_id, float up_threshold, float down_threshold);

int edge_submit_task(uint32_t runtime_id, uint32_t deployment_id, const char *task_name,
                     TaskPriority priority, uint64_t timeout_ms);
int edge_get_task_status(uint32_t runtime_id, uint64_t task_id, TaskState *state);
int edge_cancel_task(uint32_t runtime_id, uint64_t task_id);
int edge_get_task_result(uint32_t runtime_id, uint64_t task_id, char *result, uint32_t result_len);

int edge_record_metric(uint32_t runtime_id, uint32_t container_id, const char *metric_name, float value);
int edge_get_metrics(uint32_t runtime_id, uint32_t container_id, EdgeMetric *metrics, uint32_t *count);
int edge_get_container_resource_usage(uint32_t runtime_id, uint32_t container_id, ResourceLimit *usage);

int edge_allocate_resources(uint32_t runtime_id, uint32_t container_id, ResourceLimit *limit);
int edge_deallocate_resources(uint32_t runtime_id, uint32_t container_id);
int edge_migrate_container(uint32_t src_runtime_id, uint32_t dst_runtime_id, uint32_t container_id);

int edge_set_connected(uint32_t runtime_id, uint32_t is_connected);
int edge_update_health_score(uint32_t runtime_id, float health);
int edge_get_runtime_stats(uint32_t runtime_id, uint64_t *tasks_executed, uint64_t *tasks_failed, uint64_t *bytes_processed);

EdgeRuntime *edge_get_runtime_info(uint32_t runtime_id);
EdgeContainer *edge_get_container_info(uint32_t runtime_id, uint32_t container_id);
EdgeDeployment *edge_get_deployment_info(uint32_t runtime_id, uint32_t deployment_id);

int edge_export_runtime_state(uint32_t runtime_id, const char *format);

#endif
