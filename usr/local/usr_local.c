#include "usr_local.h"

int usr_local_init(void)
{
    return 0;
}

int usr_local_list_resources(usr_local_entry_t *resources, uint32_t *count)
{
    if (!resources || !count)
    {
        return -1;
    }
    return 0;
}

int usr_local_get_resource(const char *name, usr_local_entry_t *resource)
{
    if (!name || !resource)
    {
        return -1;
    }
    return 0;
}

int usr_local_add_resource(const char *name, const char *type, const char *path)
{
    if (!name || !type || !path)
    {
        return -1;
    }
    return 0;
}

int usr_local_remove_resource(const char *name)
{
    if (!name)
    {
        return -1;
    }
    return 0;
}

int usr_local_get_state(usr_local_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
