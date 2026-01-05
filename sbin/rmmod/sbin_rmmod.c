#include "sbin_rmmod.h"

int sbin_rmmod_init(void)
{
    return 0;
}

int sbin_rmmod_remove(sbin_rmmod_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_rmmod_check_dependencies(const char *module_name)
{
    if (!module_name)
    {
        return -1;
    }
    return 0;
}

int sbin_rmmod_get_status(uint32_t rmmod_id, sbin_rmmod_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_rmmod_get_state(sbin_rmmod_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
