#include "usr_share.h"

int usr_share_init(void)
{
    return 0;
}

int usr_share_list_resources(usr_share_entry_t *resources, uint32_t *count)
{
    if (!resources || !count)
    {
        return -1;
    }
    return 0;
}

int usr_share_get_resource(const char *name, usr_share_entry_t *resource)
{
    if (!name || !resource)
    {
        return -1;
    }
    return 0;
}

int usr_share_search_by_category(const char *category, usr_share_entry_t *resources, uint32_t *count)
{
    if (!category || !resources || !count)
    {
        return -1;
    }
    return 0;
}

int usr_share_get_locales(char **locales, uint32_t *count)
{
    if (!locales || !count)
    {
        return -1;
    }
    return 0;
}

int usr_share_get_state(usr_share_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
