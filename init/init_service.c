#include "init_service.h"

int init_service_register(init_service_descriptor_t *descriptor)
{
    if (!descriptor)
    {
        return -1;
    }
    return 0;
}

int init_service_unregister(uint32_t service_id)
{
    return 0;
}

int init_service_start(uint32_t service_id)
{
    return 0;
}

int init_service_stop(uint32_t service_id)
{
    return 0;
}

int init_service_restart(uint32_t service_id)
{
    return 0;
}

int init_service_get_status(uint32_t service_id)
{
    return 0;
}
