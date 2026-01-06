#include "sbin.h"

int sbin_init(void)
{
    return 0;
}

int sbin_start_service(const char *service)
{
    if (!service)
    {
        return -1;
    }
    return 0;
}

int sbin_stop_service(const char *service)
{
    if (!service)
    {
        return -1;
    }
    return 0;
}

int sbin_restart_service(const char *service)
{
    if (!service)
    {
        return -1;
    }
    return 0;
}

int sbin_get_service_status(const char *service, uint32_t *status)
{
    if (!service || !status)
    {
        return -1;
    }
    *status = 0;
    return 0;
}

int sbin_enable_service(const char *service)
{
    if (!service)
    {
        return -1;
    }
    return 0;
}
