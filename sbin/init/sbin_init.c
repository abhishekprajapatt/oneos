#include "sbin_init.h"

int sbin_init_init(void)
{
    return 0;
}

int sbin_init_set_runlevel(uint32_t runlevel)
{
    return 0;
}

int sbin_init_get_runlevel(uint32_t *runlevel)
{
    if (!runlevel)
    {
        return -1;
    }
    return 0;
}

int sbin_init_spawn_service(const char *service)
{
    if (!service)
    {
        return -1;
    }
    return 0;
}

int sbin_init_get_state(sbin_init_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
