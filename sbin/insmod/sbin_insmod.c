#include "sbin_insmod.h"

int sbin_insmod_init(void)
{
    return 0;
}

int sbin_insmod_insert(sbin_insmod_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_insmod_check_module(const char *module_path)
{
    if (!module_path)
    {
        return -1;
    }
    return 0;
}

int sbin_insmod_get_status(uint32_t module_id, sbin_insmod_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_insmod_get_state(sbin_insmod_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
