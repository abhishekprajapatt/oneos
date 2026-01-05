#include "sbin_updatealternatives.h"

int sbin_updatealternatives_init(void)
{
    return 0;
}

int sbin_updatealternatives_install(sbin_updatealternatives_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_updatealternatives_remove(const char *link_name)
{
    if (!link_name)
    {
        return -1;
    }
    return 0;
}

int sbin_updatealternatives_get_status(const char *link_name, sbin_updatealternatives_entry_t *entry)
{
    if (!link_name || !entry)
    {
        return -1;
    }
    return 0;
}

int sbin_updatealternatives_get_state(sbin_updatealternatives_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
