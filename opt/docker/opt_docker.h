#ifndef OPT_DOCKER_H
#define OPT_DOCKER_H

#include <stdint.h>

typedef struct
{
    uint32_t container_id;
    const char *container_name;
    const char *image_id;
    uint32_t status;
    uint64_t memory_usage;
} opt_docker_container_t;

typedef struct
{
    uint32_t total_containers;
    uint32_t running_containers;
    uint32_t stopped_containers;
    uint64_t total_image_size;
} opt_docker_state_t;

int opt_docker_init(void);
int opt_docker_create_container(opt_docker_container_t *container);
int opt_docker_start_container(uint32_t container_id);
int opt_docker_stop_container(uint32_t container_id);
int opt_docker_remove_container(uint32_t container_id);
int opt_docker_get_container_info(uint32_t container_id, opt_docker_container_t *container);
int opt_docker_get_state(opt_docker_state_t *state);

#endif
