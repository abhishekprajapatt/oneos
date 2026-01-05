#include "sbin_lsmod.h"

int sbin_lsmod_init(void)
{
    return 0;
}

int sbin_lsmod_list(sbin_lsmod_entry_t *modules, uint32_t *count)
{
    if (!modules || !count)
    {
        return -1;
    }
    return 0;
}

int sbin_lsmod_get_module(const char *name, sbin_lsmod_entry_t *entry)
{
    if (!name || !entry)
    {
        return -1;
    }
    return 0;
}

int sbin_lsmod_get_dependencies(const char *module, char **deps, uint32_t *count)
{
    if (!module || !deps || !count)
    {
        return -1;
    }
    return 0;
}

int sbin_lsmod_get_state(sbin_lsmod_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
