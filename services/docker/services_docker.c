#include "services_docker.h"

int services_docker_init(void)
{
    return 0;
}

int services_docker_create_container(services_docker_container_t *container)
{
    if (!container)
    {
        return -1;
    }
    return 0;
}

int services_docker_start_container(uint32_t container_id)
{
    return 0;
}

int services_docker_stop_container(uint32_t container_id)
{
    return 0;
}

int services_docker_remove_container(uint32_t container_id)
{
    return 0;
}

int services_docker_get_container_logs(uint32_t container_id, char *logs, uint32_t *size)
{
    if (!logs || !size)
    {
        return -1;
    }
    return 0;
}

int services_docker_get_state(services_docker_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
