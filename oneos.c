#include "oneos.h"

int oneos_init(void)
{
    return 0;
}

int oneos_start(void)
{
    return 0;
}

int oneos_shutdown(void)
{
    return 0;
}

int oneos_get_version(char *version, uint32_t max_len)
{
    if (!version || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int oneos_get_system_info(void)
{
    return 0;
}

int oneos_load_configuration(const char *config_file)
{
    if (!config_file)
    {
        return -1;
    }
    return 0;
}
