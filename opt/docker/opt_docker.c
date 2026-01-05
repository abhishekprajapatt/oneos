#include "opt_docker.h"

int opt_docker_init(void)
{
    return 0;
}

int opt_docker_create_container(opt_docker_container_t *container)
{
    if (!container)
    {
        return -1;
    }
    return 0;
}

int opt_docker_start_container(uint32_t container_id)
{
    return 0;
}

int opt_docker_stop_container(uint32_t container_id)
{
    return 0;
}

int opt_docker_remove_container(uint32_t container_id)
{
    return 0;
}

int opt_docker_get_container_info(uint32_t container_id, opt_docker_container_t *container)
{
    if (!container)
    {
        return -1;
    }
    return 0;
}

int opt_docker_get_state(opt_docker_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
