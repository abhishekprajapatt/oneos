#include "container_management.h"
#include <stdlib.h>
#include <string.h>

// Global container manager instance
static ContainerManager *global_cm = NULL;
static uint32_t next_container_id = 0;

ContainerManager *container_manager_init(void)
{
    ContainerManager *cm = (ContainerManager *)malloc(sizeof(ContainerManager));
    if (!cm)
        return NULL;

    memset(cm, 0, sizeof(ContainerManager));
    cm->manager_id = 1;
    cm->container_count = 0;
    cm->image_count = 0;
    cm->registry_count = 0;
    cm->total_containers_created = 0;
    cm->total_containers_stopped = 0;

    global_cm = cm;
    return cm;
}

int container_manager_destroy(ContainerManager *cm)
{
    if (!cm)
        return -1;

    for (uint32_t i = 0; i < cm->container_count; i++)
    {
        if (cm->containers[i].state == CONTAINER_STATE_RUNNING)
            container_stop(&cm->containers[i]);
    }

    free(cm);
    global_cm = NULL;
    return 0;
}

Container *container_create(ContainerManager *cm, const char *name, const char *image_name)
{
    if (!cm || cm->container_count >= MAX_CONTAINERS || !name || !image_name)
        return NULL;

    Container *container = &cm->containers[cm->container_count];
    memset(container, 0, sizeof(Container));

    container->container_id = next_container_id++;
    strncpy(container->name, name, MAX_CONTAINER_NAME_LEN - 1);
    strncpy(container->image_name, image_name, 127);
    strncpy(container->image_version, "latest", 31);

    container->state = CONTAINER_STATE_CREATED;
    container->memory_limit = 512 * 1024 * 1024; // 512MB
    container->cpu_shares = 1024;
    container->volume_count = 0;
    container->port_mapping_count = 0;
    container->env_var_count = 0;
    container->created_timestamp = 0;
    container->uptime = 0;
    container->cpu_usage_ns = 0;
    container->memory_usage = 0;

    cm->container_count++;
    cm->total_containers_created++;

    return container;
}

int container_start(Container *container)
{
    if (!container || container->state == CONTAINER_STATE_RUNNING)
        return -1;

    container->state = CONTAINER_STATE_RUNNING;
    container->started_timestamp = 0;
    container->pid = (uint64_t)container; // Simulate PID assignment
    container->uptime = 0;

    return 0;
}

int container_stop(Container *container)
{
    if (!container || (container->state != CONTAINER_STATE_RUNNING &&
                       container->state != CONTAINER_STATE_PAUSED))
        return -1;

    container->state = CONTAINER_STATE_STOPPED;
    container->pid = 0;

    return 0;
}

int container_pause(Container *container)
{
    if (!container || container->state != CONTAINER_STATE_RUNNING)
        return -1;

    container->state = CONTAINER_STATE_PAUSED;
    return 0;
}

int container_resume(Container *container)
{
    if (!container || container->state != CONTAINER_STATE_PAUSED)
        return -1;

    container->state = CONTAINER_STATE_RUNNING;
    return 0;
}

int container_delete(ContainerManager *cm, uint32_t container_id)
{
    if (!cm)
        return -1;

    for (uint32_t i = 0; i < cm->container_count; i++)
    {
        if (cm->containers[i].container_id == container_id)
        {
            if (cm->containers[i].state == CONTAINER_STATE_RUNNING)
                container_stop(&cm->containers[i]);
            return 0;
        }
    }

    return -1;
}

int container_add_volume(Container *container, const char *source, const char *dest)
{
    if (!container || container->volume_count >= MAX_CONTAINER_VOLUMES || !source || !dest)
        return -1;

    Volume *vol = &container->volumes[container->volume_count];
    strncpy(vol->source, source, 127);
    strncpy(vol->destination, dest, 127);
    vol->read_only = 0;

    container->volume_count++;
    return 0;
}

int container_remove_volume(Container *container, uint32_t volume_index)
{
    if (!container || volume_index >= container->volume_count)
        return -1;

    if (volume_index < container->volume_count - 1)
    {
        memcpy(&container->volumes[volume_index],
               &container->volumes[volume_index + 1],
               sizeof(Volume) * (container->volume_count - volume_index - 1));
    }

    container->volume_count--;
    return 0;
}

int container_add_port_mapping(Container *container, uint32_t container_port, uint32_t host_port)
{
    if (!container || container->port_mapping_count >= MAX_CONTAINER_PORTS)
        return -1;

    PortMapping *pm = &container->port_mappings[container->port_mapping_count];
    pm->container_port = container_port;
    pm->host_port = host_port;
    strncpy(pm->protocol, "tcp", 7);

    container->port_mapping_count++;
    return 0;
}

int container_remove_port_mapping(Container *container, uint32_t port_index)
{
    if (!container || port_index >= container->port_mapping_count)
        return -1;

    if (port_index < container->port_mapping_count - 1)
    {
        memcpy(&container->port_mappings[port_index],
               &container->port_mappings[port_index + 1],
               sizeof(PortMapping) * (container->port_mapping_count - port_index - 1));
    }

    container->port_mapping_count--;
    return 0;
}

int container_set_env(Container *container, const char *key, const char *value)
{
    if (!container || container->env_var_count >= MAX_CONTAINER_ENV_VARS || !key || !value)
        return -1;

    EnvironmentVariable *env = &container->env_vars[container->env_var_count];
    strncpy(env->key, key, 63);
    strncpy(env->value, value, 255);

    container->env_var_count++;
    return 0;
}

int container_unset_env(Container *container, const char *key)
{
    if (!container || !key)
        return -1;

    for (uint32_t i = 0; i < container->env_var_count; i++)
    {
        if (strcmp(container->env_vars[i].key, key) == 0)
        {
            if (i < container->env_var_count - 1)
            {
                memcpy(&container->env_vars[i],
                       &container->env_vars[i + 1],
                       sizeof(EnvironmentVariable) * (container->env_var_count - i - 1));
            }
            container->env_var_count--;
            return 0;
        }
    }

    return -1;
}

ContainerImage *container_image_pull(ContainerManager *cm, const char *name, const char *version)
{
    if (!cm || cm->image_count >= MAX_CONTAINERS || !name || !version)
        return NULL;

    ContainerImage *image = &cm->images[cm->image_count];
    memset(image, 0, sizeof(ContainerImage));

    image->image_id = cm->image_count;
    strncpy(image->name, name, 127);
    strncpy(image->version, version, 31);
    image->build_status = IMAGE_STATUS_PULLED;
    image->size = 512 * 1024 * 1024; // 512MB default
    image->layer_count = 5;
    image->created_timestamp = 0;

    cm->image_count++;
    return image;
}

int container_image_build(ContainerManager *cm, const char *dockerfile_path)
{
    if (!cm || !dockerfile_path)
        return -1;

    // Simulate Docker build process
    return 0;
}

int container_image_delete(ContainerManager *cm, uint32_t image_id)
{
    if (!cm || image_id >= cm->image_count)
        return -1;

    return 0;
}

int container_registry_login(ContainerManager *cm, const char *registry_url)
{
    if (!cm || cm->registry_count >= 16 || !registry_url)
        return -1;

    ContainerRegistry *reg = &cm->registries[cm->registry_count];
    memset(reg, 0, sizeof(ContainerRegistry));

    reg->registry_id = cm->registry_count;
    strncpy(reg->registry_url, registry_url, 255);
    reg->authenticated = 1;

    cm->registry_count++;
    return 0;
}

int container_registry_logout(ContainerManager *cm, uint32_t registry_id)
{
    if (!cm || registry_id >= cm->registry_count)
        return -1;

    cm->registries[registry_id].authenticated = 0;
    return 0;
}

int container_get_metrics(Container *container, ContainerMetrics *metrics)
{
    if (!container || !metrics)
        return -1;

    metrics->container_id = container->container_id;
    metrics->cpu_usage_percent = (container->state == CONTAINER_STATE_RUNNING) ? 25.5f : 0.0f;
    metrics->memory_used = container->memory_usage;
    metrics->disk_io_read = container->disk_io_read;
    metrics->disk_io_write = container->disk_io_write;
    metrics->network_connections = container->port_mapping_count;

    return 0;
}

int container_manager_get_metrics(ContainerManager *cm)
{
    if (!cm)
        return -1;

    for (uint32_t i = 0; i < cm->container_count; i++)
    {
        ContainerMetrics metrics;
        container_get_metrics(&cm->containers[i], &metrics);
    }

    return 0;
}
