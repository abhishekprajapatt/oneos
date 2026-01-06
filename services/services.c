#include "services.h"

int services_init(void)
{
    return 0;
}

int services_register(const char *name, void (*handler)(void))
{
    if (!name || !handler)
    {
        return -1;
    }
    return 0;
}

int services_start(const char *name)
{
    if (!name)
    {
        return -1;
    }
    return 0;
}

int services_stop(const char *name)
{
    if (!name)
    {
        return -1;
    }
    return 0;
}

int services_get_status(const char *name, service_t *service)
{
    if (!name || !service)
    {
        return -1;
    }
    service->service_id = 0;
    service->status = 0;
    return 0;
}

int services_restart(const char *name)
{
    if (!name)
    {
        return -1;
    }
    return 0;
}
