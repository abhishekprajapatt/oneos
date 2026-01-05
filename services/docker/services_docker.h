#ifndef SERVICES_DOCKER_H
#define SERVICES_DOCKER_H

#include <stdint.h>

typedef struct
{
    uint32_t container_id;
    const char *container_name;
    const char *image_id;
    uint32_t container_state;
    uint64_t memory_limit;
} services_docker_container_t;

typedef struct
{
    uint32_t total_containers;
    uint32_t running_containers;
    uint32_t stopped_containers;
    uint64_t total_memory_used;
} services_docker_state_t;

int services_docker_init(void);
int services_docker_create_container(services_docker_container_t *container);
int services_docker_start_container(uint32_t container_id);
int services_docker_stop_container(uint32_t container_id);
int services_docker_remove_container(uint32_t container_id);
int services_docker_get_container_logs(uint32_t container_id, char *logs, uint32_t *size);
int services_docker_get_state(services_docker_state_t *state);

#endif
