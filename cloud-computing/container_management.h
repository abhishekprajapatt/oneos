#ifndef CONTAINER_MANAGEMENT_H
#define CONTAINER_MANAGEMENT_H

#include <stdint.h>
#include <stddef.h>

#define MAX_CONTAINERS 256
#define MAX_CONTAINER_NAME_LEN 64
#define MAX_CONTAINER_VOLUMES 16
#define MAX_CONTAINER_PORTS 32
#define MAX_CONTAINER_ENV_VARS 64

typedef enum
{
    CONTAINER_STATE_CREATED,
    CONTAINER_STATE_RUNNING,
    CONTAINER_STATE_PAUSED,
    CONTAINER_STATE_STOPPED,
    CONTAINER_STATE_EXITED,
    CONTAINER_STATE_DEAD
} ContainerState;

typedef enum
{
    IMAGE_STATUS_NONE,
    IMAGE_STATUS_PULLING,
    IMAGE_STATUS_PULLED,
    IMAGE_STATUS_BUILDING,
    IMAGE_STATUS_BUILT,
    IMAGE_STATUS_ERROR
} ImageBuildStatus;

typedef struct
{
    char source[128];
    char destination[128];
    uint8_t read_only;
} Volume;

typedef struct
{
    uint32_t container_port;
    uint32_t host_port;
    char protocol[8]; // tcp, udp
} PortMapping;

typedef struct
{
    char key[64];
    char value[256];
} EnvironmentVariable;

typedef struct
{
    uint32_t container_id;
    char name[MAX_CONTAINER_NAME_LEN];
    char image_name[128];
    char image_version[32];

    ContainerState state;
    uint64_t pid; // Container process ID

    uint64_t memory_limit;
    uint32_t cpu_shares;

    Volume volumes[MAX_CONTAINER_VOLUMES];
    uint32_t volume_count;

    PortMapping port_mappings[MAX_CONTAINER_PORTS];
    uint32_t port_mapping_count;

    EnvironmentVariable env_vars[MAX_CONTAINER_ENV_VARS];
    uint32_t env_var_count;

    uint32_t created_timestamp;
    uint32_t started_timestamp;
    uint32_t uptime;

    uint64_t cpu_usage_ns;
    uint64_t memory_usage;
    uint64_t disk_io_read;
    uint64_t disk_io_write;
} Container;

typedef struct
{
    uint32_t image_id;
    char name[128];
    char version[32];
    char registry[128];

    uint64_t size;
    uint32_t layer_count;
    ImageBuildStatus build_status;
    uint32_t created_timestamp;
} ContainerImage;

typedef struct
{
    uint32_t registry_id;
    char registry_url[256];
    char username[64];
    uint8_t authenticated;
} ContainerRegistry;

typedef struct
{
    uint32_t manager_id;

    uint32_t container_count;
    Container containers[MAX_CONTAINERS];

    uint32_t image_count;
    ContainerImage images[MAX_CONTAINERS];

    uint32_t registry_count;
    ContainerRegistry registries[16];

    uint64_t total_containers_created;
    uint64_t total_containers_stopped;
} ContainerManager;

// Container Manager lifecycle
ContainerManager *container_manager_init(void);
int container_manager_destroy(ContainerManager *cm);

// Container operations
Container *container_create(ContainerManager *cm, const char *name, const char *image_name);
int container_start(Container *container);
int container_stop(Container *container);
int container_pause(Container *container);
int container_resume(Container *container);
int container_delete(ContainerManager *cm, uint32_t container_id);

// Volume management
int container_add_volume(Container *container, const char *source, const char *dest);
int container_remove_volume(Container *container, uint32_t volume_index);

// Port mapping
int container_add_port_mapping(Container *container, uint32_t container_port, uint32_t host_port);
int container_remove_port_mapping(Container *container, uint32_t port_index);

// Environment variables
int container_set_env(Container *container, const char *key, const char *value);
int container_unset_env(Container *container, const char *key);

// Image operations
ContainerImage *container_image_pull(ContainerManager *cm, const char *name, const char *version);
int container_image_build(ContainerManager *cm, const char *dockerfile_path);
int container_image_delete(ContainerManager *cm, uint32_t image_id);

// Registry operations
int container_registry_login(ContainerManager *cm, const char *registry_url);
int container_registry_logout(ContainerManager *cm, uint32_t registry_id);

// Monitoring
typedef struct
{
    uint32_t container_id;
    float cpu_usage_percent;
    uint64_t memory_used;
    uint64_t disk_io_read;
    uint64_t disk_io_write;
    uint32_t network_connections;
} ContainerMetrics;

int container_get_metrics(Container *container, ContainerMetrics *metrics);
int container_manager_get_metrics(ContainerManager *cm);

#endif // CONTAINER_MANAGEMENT_H
