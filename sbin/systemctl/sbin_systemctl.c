#include "sbin_systemctl.h"

int sbin_systemctl_init(void)
{
    return 0;
}

int sbin_systemctl_start_service(const char *service_name)
{
    if (!service_name)
    {
        return -1;
    }
    return 0;
}

int sbin_systemctl_stop_service(const char *service_name)
{
    if (!service_name)
    {
        return -1;
    }
    return 0;
}

int sbin_systemctl_restart_service(const char *service_name)
{
    if (!service_name)
    {
        return -1;
    }
    return 0;
}

int sbin_systemctl_enable_service(const char *service_name)
{
    if (!service_name)
    {
        return -1;
    }
    return 0;
}

int sbin_systemctl_disable_service(const char *service_name)
{
    if (!service_name)
    {
        return -1;
    }
    return 0;
}

int sbin_systemctl_get_service_status(const char *service_name, sbin_systemctl_service_t *service)
{
    if (!service_name || !service)
    {
        return -1;
    }
    return 0;
}

int sbin_systemctl_get_state(sbin_systemctl_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
