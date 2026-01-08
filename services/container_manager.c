#include "container_manager.h"
#include <stdlib.h>
#include <string.h>

ContainerManager *container_init(void)
{
    ContainerManager *manager = (ContainerManager *)malloc(sizeof(ContainerManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(ContainerManager));
    manager->container_count = 0;
    manager->image_count = 0;
    manager->volume_count = 0;
    manager->resource_limit_count = 0;
    manager->total_container_memory_mb = 0;
    manager->total_volume_size_mb = 0;
    manager->running_container_count = 0;

    return manager;
}

void container_cleanup(ContainerManager *manager)
{
    if (!manager)
        return;

    for (uint32_t i = 0; i < manager->container_count; i++)
    {
        if (manager->containers[i].state == CONTAINER_STATE_RUNNING)
        {
            container_stop(manager, manager->containers[i].container_id);
        }
    }

    memset(manager, 0, sizeof(ContainerManager));
    free(manager);
}

uint32_t container_create(ContainerManager *manager, const char *name, const char *image_id,
                          uint32_t cpu_limit, uint32_t memory_mb)
{
    if (!manager || manager->container_count >= MAX_CONTAINERS)
        return 0;
    if (!name || !image_id)
        return 0;

    Container *container = &manager->containers[manager->container_count];
    container->container_id = manager->container_count + 1;
    strncpy(container->name, name, 127);
    container->name[127] = '\0';
    strncpy(container->image_id, image_id, 63);
    container->image_id[63] = '\0';
    container->state = CONTAINER_STATE_CREATED;
    container->cpu_limit_percent = cpu_limit;
    container->memory_limit_mb = memory_mb;
    container->pids_limit = 1024;
    container->port_count = 0;
    container->env_var_count = 0;
    strncpy(container->working_directory, "/", 255);
    container->container_size_mb = 100;
    container->memory_usage_mb = 0;
    container->cpu_time_ms = 0;
    container->created_time = time(NULL);
    container->started_time = 0;
    container->stopped_time = 0;

    manager->total_container_memory_mb += memory_mb;
    manager->container_count++;

    return container->container_id;
}

int container_delete(ContainerManager *manager, uint32_t container_id)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->container_count; i++)
    {
        if (manager->containers[i].container_id == container_id)
        {
            if (manager->containers[i].state == CONTAINER_STATE_RUNNING)
            {
                container_stop(manager, container_id);
            }
            manager->total_container_memory_mb -= manager->containers[i].memory_limit_mb;
            memmove(&manager->containers[i], &manager->containers[i + 1],
                    (manager->container_count - i - 1) * sizeof(Container));
            manager->container_count--;
            return 0;
        }
    }
    return -1;
}

int container_start(ContainerManager *manager, uint32_t container_id)
{
    if (!manager)
        return -1;

    Container *container = container_get(manager, container_id);
    if (!container || container->state == CONTAINER_STATE_RUNNING)
        return -1;

    container->state = CONTAINER_STATE_RUNNING;
    container->started_time = time(NULL);
    container->stopped_time = 0;
    manager->running_container_count++;

    return 0;
}

int container_stop(ContainerManager *manager, uint32_t container_id)
{
    if (!manager)
        return -1;

    Container *container = container_get(manager, container_id);
    if (!container || container->state == CONTAINER_STATE_STOPPED)
        return -1;

    container->state = CONTAINER_STATE_STOPPED;
    container->stopped_time = time(NULL);
    if (manager->running_container_count > 0)
    {
        manager->running_container_count--;
    }

    return 0;
}

int container_pause(ContainerManager *manager, uint32_t container_id)
{
    if (!manager)
        return -1;

    Container *container = container_get(manager, container_id);
    if (!container || container->state != CONTAINER_STATE_RUNNING)
        return -1;

    container->state = CONTAINER_STATE_PAUSED;
    return 0;
}

int container_resume(ContainerManager *manager, uint32_t container_id)
{
    if (!manager)
        return -1;

    Container *container = container_get(manager, container_id);
    if (!container || container->state != CONTAINER_STATE_PAUSED)
        return -1;

    container->state = CONTAINER_STATE_RUNNING;
    return 0;
}

int container_restart(ContainerManager *manager, uint32_t container_id)
{
    if (!manager)
        return -1;

    Container *container = container_get(manager, container_id);
    if (!container)
        return -1;

    container->state = CONTAINER_STATE_RESTARTING;
    container->started_time = time(NULL);
    container->state = CONTAINER_STATE_RUNNING;

    return 0;
}

int container_add_port_mapping(ContainerManager *manager, uint32_t container_id,
                               uint16_t container_port, uint16_t host_port, const char *protocol)
{
    if (!manager || !protocol)
        return -1;

    Container *container = container_get(manager, container_id);
    if (!container || container->port_count >= MAX_CONTAINER_PORTS)
        return -1;

    PortMapping *mapping = &container->ports[container->port_count];
    mapping->container_port = container_port;
    mapping->host_port = host_port;
    strncpy(mapping->protocol, protocol, 15);
    mapping->protocol[15] = '\0';

    container->port_count++;
    return 0;
}

int container_remove_port_mapping(ContainerManager *manager, uint32_t container_id, uint16_t host_port)
{
    if (!manager)
        return -1;

    Container *container = container_get(manager, container_id);
    if (!container)
        return -1;

    for (uint32_t i = 0; i < container->port_count; i++)
    {
        if (container->ports[i].host_port == host_port)
        {
            memmove(&container->ports[i], &container->ports[i + 1],
                    (container->port_count - i - 1) * sizeof(PortMapping));
            container->port_count--;
            return 0;
        }
    }
    return -1;
}

int container_set_env_var(ContainerManager *manager, uint32_t container_id,
                          const char *key, const char *value)
{
    if (!manager || !key || !value)
        return -1;

    Container *container = container_get(manager, container_id);
    if (!container || container->env_var_count >= MAX_ENV_VARS)
        return -1;

    for (uint32_t i = 0; i < container->env_var_count; i++)
    {
        if (strcmp(container->env_vars[i].key, key) == 0)
        {
            strncpy(container->env_vars[i].value, value, 255);
            container->env_vars[i].value[255] = '\0';
            return 0;
        }
    }

    EnvVar *env = &container->env_vars[container->env_var_count];
    strncpy(env->key, key, 127);
    env->key[127] = '\0';
    strncpy(env->value, value, 255);
    env->value[255] = '\0';

    container->env_var_count++;
    return 0;
}

int container_unset_env_var(ContainerManager *manager, uint32_t container_id, const char *key)
{
    if (!manager || !key)
        return -1;

    Container *container = container_get(manager, container_id);
    if (!container)
        return -1;

    for (uint32_t i = 0; i < container->env_var_count; i++)
    {
        if (strcmp(container->env_vars[i].key, key) == 0)
        {
            memmove(&container->env_vars[i], &container->env_vars[i + 1],
                    (container->env_var_count - i - 1) * sizeof(EnvVar));
            container->env_var_count--;
            return 0;
        }
    }
    return -1;
}

uint32_t container_image_pull(ContainerManager *manager, const char *name, const char *tag)
{
    if (!manager || manager->image_count >= MAX_IMAGES || !name)
        return 0;

    ContainerImage *image = &manager->images[manager->image_count];
    image->image_id = manager->image_count + 1;
    strncpy(image->name, name, 127);
    image->name[127] = '\0';
    strncpy(image->tag, tag ? tag : "latest", 63);
    image->tag[63] = '\0';
    image->status = IMAGE_STATUS_READY;
    image->layer_count = 5;
    image->size_mb = 256;
    sprintf(image->digest, "sha256:%032x", image->image_id);
    image->pull_count = 1;
    image->created_time = time(NULL);
    image->last_used_time = time(NULL);

    manager->image_count++;
    return image->image_id;
}

int container_image_remove(ContainerManager *manager, uint32_t image_id)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->image_count; i++)
    {
        if (manager->images[i].image_id == image_id)
        {
            memmove(&manager->images[i], &manager->images[i + 1],
                    (manager->image_count - i - 1) * sizeof(ContainerImage));
            manager->image_count--;
            return 0;
        }
    }
    return -1;
}

int container_image_build(ContainerManager *manager, const char *dockerfile_path,
                          const char *name, const char *tag)
{
    if (!manager || !dockerfile_path || !name)
        return -1;
    return container_image_pull(manager, name, tag) ? 0 : -1;
}

ContainerImage *container_image_get(ContainerManager *manager, uint32_t image_id)
{
    if (!manager)
        return NULL;

    for (uint32_t i = 0; i < manager->image_count; i++)
    {
        if (manager->images[i].image_id == image_id)
        {
            manager->images[i].last_used_time = time(NULL);
            return &manager->images[i];
        }
    }
    return NULL;
}

uint32_t container_volume_create(ContainerManager *manager, const char *name,
                                 uint64_t size_mb, bool persistent)
{
    if (!manager || manager->volume_count >= MAX_VOLUMES || !name)
        return 0;

    Volume *volume = &manager->volumes[manager->volume_count];
    volume->volume_id = manager->volume_count + 1;
    strncpy(volume->name, name, 127);
    volume->name[127] = '\0';
    strncpy(volume->mount_path, "/mnt/volumes/", 255);
    strncat(volume->mount_path, name, 255 - strlen(volume->mount_path) - 1);
    volume->size_mb = size_mb;
    volume->used_mb = 0;
    volume->is_persistent = persistent;
    volume->reference_count = 0;
    volume->created_time = time(NULL);

    manager->total_volume_size_mb += size_mb;
    manager->volume_count++;

    return volume->volume_id;
}

int container_volume_delete(ContainerManager *manager, uint32_t volume_id)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->volume_count; i++)
    {
        if (manager->volumes[i].volume_id == volume_id)
        {
            if (manager->volumes[i].reference_count > 0)
                return -1;
            manager->total_volume_size_mb -= manager->volumes[i].size_mb;
            memmove(&manager->volumes[i], &manager->volumes[i + 1],
                    (manager->volume_count - i - 1) * sizeof(Volume));
            manager->volume_count--;
            return 0;
        }
    }
    return -1;
}

int container_volume_mount(ContainerManager *manager, uint32_t container_id,
                           uint32_t volume_id, const char *mount_path)
{
    if (!manager || !mount_path)
        return -1;

    Container *container = container_get(manager, container_id);
    Volume *volume = container_volume_get(manager, volume_id);

    if (!container || !volume)
        return -1;

    volume->reference_count++;
    return 0;
}

int container_volume_unmount(ContainerManager *manager, uint32_t container_id, uint32_t volume_id)
{
    if (!manager)
        return -1;

    Volume *volume = container_volume_get(manager, volume_id);
    if (!volume || volume->reference_count == 0)
        return -1;

    volume->reference_count--;
    return 0;
}

Volume *container_volume_get(ContainerManager *manager, uint32_t volume_id)
{
    if (!manager)
        return NULL;

    for (uint32_t i = 0; i < manager->volume_count; i++)
    {
        if (manager->volumes[i].volume_id == volume_id)
        {
            return &manager->volumes[i];
        }
    }
    return NULL;
}

Container *container_get(ContainerManager *manager, uint32_t container_id)
{
    if (!manager)
        return NULL;

    for (uint32_t i = 0; i < manager->container_count; i++)
    {
        if (manager->containers[i].container_id == container_id)
        {
            return &manager->containers[i];
        }
    }
    return NULL;
}

int container_set_resource_limit(ContainerManager *manager, uint32_t container_id,
                                 const char *resource_type, uint64_t limit)
{
    if (!manager || !resource_type)
        return -1;

    Container *container = container_get(manager, container_id);
    if (!container)
        return -1;

    for (uint32_t i = 0; i < manager->resource_limit_count; i++)
    {
        if (manager->resource_limits[i].container_id == container_id &&
            strcmp(manager->resource_limits[i].resource_type, resource_type) == 0)
        {
            manager->resource_limits[i].resource_limit = limit;
            return 0;
        }
    }

    if (manager->resource_limit_count >= MAX_CONTAINERS * 4)
        return -1;

    ResourceLimit *rl = &manager->resource_limits[manager->resource_limit_count];
    rl->resource_id = manager->resource_limit_count + 1;
    rl->container_id = container_id;
    strncpy(rl->resource_type, resource_type, 63);
    rl->resource_type[63] = '\0';
    rl->resource_limit = limit;
    rl->resource_current = 0;
    rl->utilization_percent = 0;

    manager->resource_limit_count++;
    return 0;
}

uint32_t container_get_resource_utilization(ContainerManager *manager, uint32_t container_id,
                                            const char *resource_type)
{
    if (!manager || !resource_type)
        return 0;

    for (uint32_t i = 0; i < manager->resource_limit_count; i++)
    {
        if (manager->resource_limits[i].container_id == container_id &&
            strcmp(manager->resource_limits[i].resource_type, resource_type) == 0)
        {
            return manager->resource_limits[i].utilization_percent;
        }
    }
    return 0;
}

uint32_t container_get_running_count(ContainerManager *manager)
{
    if (!manager)
        return 0;
    return manager->running_container_count;
}

uint64_t container_get_total_memory_usage(ContainerManager *manager)
{
    if (!manager)
        return 0;
    return manager->total_container_memory_mb;
}

uint64_t container_get_total_volume_size(ContainerManager *manager)
{
    if (!manager)
        return 0;
    return manager->total_volume_size_mb;
}
