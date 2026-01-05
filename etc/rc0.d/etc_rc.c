#include "etc_rc.h"

int etc_rc_init(void)
{
    return 0;
}

int etc_rc_register_service(etc_rc_service_t *service)
{
    if (!service)
    {
        return -1;
    }
    return 0;
}

int etc_rc_unregister_service(uint32_t service_id)
{
    return 0;
}

int etc_rc_enable_service(uint32_t service_id)
{
    return 0;
}

int etc_rc_disable_service(uint32_t service_id)
{
    return 0;
}

int etc_rc_start_service(uint32_t service_id)
{
    return 0;
}

int etc_rc_stop_service(uint32_t service_id)
{
    return 0;
}
