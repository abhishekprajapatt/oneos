#ifndef CONTAINER_ORCHESTRATOR_H
#define CONTAINER_ORCHESTRATOR_H

#include <stdint.h>
#include <stddef.h>

#define MAX_CONTAINERS 2048
#define MAX_CONTAINER_NETWORKS 256
#define MAX_CONTAINER_VOLUMES 1024
#define MAX_ORCHESTRATION_POLICIES 512

typedef enum
{
    CONTAINER_STATE_CREATED,
    CONTAINER_STATE_RUNNING,
    CONTAINER_STATE_PAUSED,
    CONTAINER_STATE_STOPPED,
    CONTAINER_STATE_EXITED
} ContainerState;

typedef enum
{
    RESTART_POLICY_NO,
    RESTART_POLICY_ALWAYS,
    RESTART_POLICY_ON_FAILURE,
    RESTART_POLICY_UNLESS_STOPPED
} RestartPolicy;

typedef struct
{
    uint32_t container_id;
    char container_name[128];
    char image_name[256];

    ContainerState state;
    RestartPolicy restart_policy;

    uint32_t cpu_limit_percent;
    uint64_t memory_limit_bytes;

    uint64_t container_start_time;
    uint64_t container_uptime_seconds;
    uint32_t restart_count;

    uint8_t is_healthy;
    uint8_t is_privileged;
} Container;

typedef struct
{
    uint32_t network_id;
    char network_name[64];

    uint32_t container_ids[MAX_CONTAINERS];
    uint32_t container_count;

    char network_subnet[32];
    uint8_t is_bridge;
    uint8_t is_overlay;
} ContainerNetwork;

typedef struct
{
    uint32_t volume_id;
    char volume_name[128];
    char volume_path[512];

    uint64_t volume_size_bytes;
    uint64_t volume_used_bytes;

    uint8_t is_persistent;
    uint8_t is_encrypted;
} ContainerVolume;

typedef struct
{
    uint32_t policy_id;
    char policy_name[128];

    uint32_t container_count_target;
    uint32_t current_container_count;

    uint32_t cpu_request_percent;
    uint64_t memory_request_bytes;

    uint8_t auto_scale_enabled;
    uint8_t health_check_enabled;
    uint32_t health_check_interval_seconds;
} OrchestrationPolicy;

typedef struct
{
    uint32_t orchestrator_id;

    Container containers[MAX_CONTAINERS];
    uint32_t container_count;

    ContainerNetwork networks[MAX_CONTAINER_NETWORKS];
    uint32_t network_count;

    ContainerVolume volumes[MAX_CONTAINER_VOLUMES];
    uint32_t volume_count;

    OrchestrationPolicy policies[MAX_ORCHESTRATION_POLICIES];
    uint32_t policy_count;

    uint64_t total_container_operations;
    uint32_t running_containers;
    uint32_t healthy_containers;
    uint32_t unhealthy_containers;
} ContainerOrchestrator;

ContainerOrchestrator *container_orchestrator_init(void);
int container_orchestrator_destroy(ContainerOrchestrator *orchestrator);

uint32_t container_create(ContainerOrchestrator *orchestrator, const char *name,
                          const char *image, uint32_t cpu_limit, uint64_t memory_limit);

int container_start(ContainerOrchestrator *orchestrator, uint32_t container_id);
int container_stop(ContainerOrchestrator *orchestrator, uint32_t container_id);
int container_pause(ContainerOrchestrator *orchestrator, uint32_t container_id);
int container_resume(ContainerOrchestrator *orchestrator, uint32_t container_id);

int container_set_restart_policy(ContainerOrchestrator *orchestrator, uint32_t container_id,
                                 RestartPolicy policy);

int container_get_state(ContainerOrchestrator *orchestrator, uint32_t container_id,
                        ContainerState *state);

int container_get_health(ContainerOrchestrator *orchestrator, uint32_t container_id,
                         uint8_t *is_healthy);

uint32_t container_network_create(ContainerOrchestrator *orchestrator, const char *network_name,
                                  const char *subnet);

int container_network_connect(ContainerOrchestrator *orchestrator, uint32_t network_id,
                              uint32_t container_id);

int container_network_disconnect(ContainerOrchestrator *orchestrator, uint32_t network_id,
                                 uint32_t container_id);

uint32_t container_volume_create(ContainerOrchestrator *orchestrator, const char *volume_name,
                                 uint64_t size_bytes);

int container_volume_attach(ContainerOrchestrator *orchestrator, uint32_t volume_id,
                            uint32_t container_id);

uint32_t orchestration_policy_create(ContainerOrchestrator *orchestrator,
                                     const char *policy_name, uint32_t target_count);

int orchestration_policy_enable_auto_scale(ContainerOrchestrator *orchestrator,
                                           uint32_t policy_id);

int orchestration_policy_enable_health_check(ContainerOrchestrator *orchestrator,
                                             uint32_t policy_id, uint32_t interval_seconds);

int orchestration_execute_scaling(ContainerOrchestrator *orchestrator, uint32_t policy_id);

int orchestration_perform_health_check(ContainerOrchestrator *orchestrator);

typedef struct
{
    uint32_t orchestrator_id;
    uint32_t total_containers;
    uint32_t running_containers;
    uint32_t healthy_containers;
    uint32_t unhealthy_containers;
    uint32_t total_networks;
    uint32_t total_volumes;
    uint32_t total_policies;
    uint64_t total_operations;
} ContainerOrchestratorMetrics;

int container_orchestrator_get_metrics(ContainerOrchestrator *orchestrator,
                                       ContainerOrchestratorMetrics *metrics);

#endif
