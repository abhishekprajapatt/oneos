#include "system.h"

int system_init(void)
{
    return 0;
}

int system_get_info(system_info_t *info)
{
    if (!info)
    {
        return -1;
    }
    return 0;
}

int system_get_resources(system_resources_t *resources)
{
    if (!resources)
    {
        return -1;
    }
    return 0;
}

int system_get_storage_info(storage_info_t *storage)
{
    if (!storage)
    {
        return -1;
    }
    return 0;
}

int system_shutdown(void)
{
    return 0;
}

int system_reboot(void)
{
    return 0;
}

int system_sleep(void)
{
    return 0;
}

int system_wake(void)
{
    return 0;
}

int system_get_uptime(char *uptime, uint32_t max_len)
{
    if (!uptime || max_len == 0)
    {
        return -1;
    }
    return 0;
}
