#ifndef CONTAINER_MGR_H
#define CONTAINER_MGR_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_CONTAINERS 1024
#define MAX_IMAGES 512
#define MAX_VOLUMES 2048
#define MAX_CONTAINER_PORTS 64
#define MAX_ENV_VARS 256

typedef enum
{
    CONTAINER_STATE_CREATED = 0,
    CONTAINER_STATE_RUNNING = 1,
    CONTAINER_STATE_PAUSED = 2,
    CONTAINER_STATE_STOPPED = 3,
    CONTAINER_STATE_RESTARTING = 4,
    CONTAINER_STATE_EXITED = 5
} ContainerState;

typedef enum
{
    IMAGE_STATUS_BUILDING = 0,
    IMAGE_STATUS_READY = 1,
    IMAGE_STATUS_CORRUPT = 2,
    IMAGE_STATUS_DEPRECATED = 3
} ImageStatus;

typedef struct
{
    uint16_t container_port;
    uint16_t host_port;
    char protocol[16];
} PortMapping;

typedef struct
{
    char key[128];
    char value[256];
} EnvVar;

typedef struct
{
    uint32_t container_id;
    char name[128];
    char image_id[64];
    ContainerState state;
    uint32_t cpu_limit_percent;
    uint32_t memory_limit_mb;
    uint32_t pids_limit;
    PortMapping ports[MAX_CONTAINER_PORTS];
    uint32_t port_count;
    EnvVar env_vars[MAX_ENV_VARS];
    uint32_t env_var_count;
    char working_directory[256];
    uint64_t container_size_mb;
    uint64_t memory_usage_mb;
    uint64_t cpu_time_ms;
    time_t created_time;
    time_t started_time;
    time_t stopped_time;
} Container;

typedef struct
{
    uint32_t image_id;
    char name[128];
    char tag[64];
    ImageStatus status;
    uint32_t layer_count;
    uint64_t size_mb;
    char digest[128];
    uint32_t pull_count;
    time_t created_time;
    time_t last_used_time;
} ContainerImage;

typedef struct
{
    uint32_t volume_id;
    char name[128];
    char mount_path[256];
    uint64_t size_mb;
    uint64_t used_mb;
    bool is_persistent;
    uint32_t reference_count;
    time_t created_time;
} Volume;

typedef struct
{
    uint32_t resource_id;
    uint32_t container_id;
    char resource_type[64];
    uint64_t resource_limit;
    uint64_t resource_current;
    uint32_t utilization_percent;
} ResourceLimit;

typedef struct
{
    Container containers[MAX_CONTAINERS];
    uint32_t container_count;
    ContainerImage images[MAX_IMAGES];
    uint32_t image_count;
    Volume volumes[MAX_VOLUMES];
    uint32_t volume_count;
    ResourceLimit resource_limits[MAX_CONTAINERS * 4];
    uint32_t resource_limit_count;
    uint64_t total_container_memory_mb;
    uint64_t total_volume_size_mb;
    uint32_t running_container_count;
} ContainerManager;

ContainerManager *container_init(void);
void container_cleanup(ContainerManager *manager);

uint32_t container_create(ContainerManager *manager, const char *name, const char *image_id,
                          uint32_t cpu_limit, uint32_t memory_mb);
int container_delete(ContainerManager *manager, uint32_t container_id);
int container_start(ContainerManager *manager, uint32_t container_id);
int container_stop(ContainerManager *manager, uint32_t container_id);
int container_pause(ContainerManager *manager, uint32_t container_id);
int container_resume(ContainerManager *manager, uint32_t container_id);
int container_restart(ContainerManager *manager, uint32_t container_id);

int container_add_port_mapping(ContainerManager *manager, uint32_t container_id,
                               uint16_t container_port, uint16_t host_port, const char *protocol);
int container_remove_port_mapping(ContainerManager *manager, uint32_t container_id, uint16_t host_port);
int container_set_env_var(ContainerManager *manager, uint32_t container_id,
                          const char *key, const char *value);
int container_unset_env_var(ContainerManager *manager, uint32_t container_id, const char *key);

uint32_t container_image_pull(ContainerManager *manager, const char *name, const char *tag);
int container_image_remove(ContainerManager *manager, uint32_t image_id);
int container_image_build(ContainerManager *manager, const char *dockerfile_path,
                          const char *name, const char *tag);
ContainerImage *container_image_get(ContainerManager *manager, uint32_t image_id);

uint32_t container_volume_create(ContainerManager *manager, const char *name,
                                 uint64_t size_mb, bool persistent);
int container_volume_delete(ContainerManager *manager, uint32_t volume_id);
int container_volume_mount(ContainerManager *manager, uint32_t container_id,
                           uint32_t volume_id, const char *mount_path);
int container_volume_unmount(ContainerManager *manager, uint32_t container_id, uint32_t volume_id);
Volume *container_volume_get(ContainerManager *manager, uint32_t volume_id);

Container *container_get(ContainerManager *manager, uint32_t container_id);

int container_set_resource_limit(ContainerManager *manager, uint32_t container_id,
                                 const char *resource_type, uint64_t limit);
uint32_t container_get_resource_utilization(ContainerManager *manager, uint32_t container_id,
                                            const char *resource_type);

uint32_t container_get_running_count(ContainerManager *manager);
uint64_t container_get_total_memory_usage(ContainerManager *manager);
uint64_t container_get_total_volume_size(ContainerManager *manager);

#endif
