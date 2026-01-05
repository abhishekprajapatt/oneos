#include "services_core.h"

int services_core_init(void)
{
    return 0;
}

int services_core_register_service(services_core_service_t *service)
{
    if (!service)
    {
        return -1;
    }
    return 0;
}

int services_core_unregister_service(uint32_t service_id)
{
    return 0;
}

int services_core_start_service(uint32_t service_id)
{
    return 0;
}

int services_core_stop_service(uint32_t service_id)
{
    return 0;
}

int services_core_restart_service(uint32_t service_id)
{
    return 0;
}

int services_core_get_service_info(uint32_t service_id, services_core_service_t *service)
{
    if (!service)
    {
        return -1;
    }
    return 0;
}

int services_core_get_state(services_core_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
