#include "frameworks.h"

int frameworks_init(void)
{
    return 0;
}

int frameworks_load(const char *framework_name)
{
    if (!framework_name)
    {
        return -1;
    }
    return 0;
}

int frameworks_unload(const char *framework_name)
{
    if (!framework_name)
    {
        return -1;
    }
    return 0;
}

int frameworks_get_symbol(const char *framework_name, const char *symbol, void **ptr)
{
    if (!framework_name || !symbol || !ptr)
    {
        return -1;
    }
    return 0;
}

int frameworks_list(framework_t **frameworks, uint32_t *count)
{
    if (!frameworks || !count)
    {
        return -1;
    }
    *count = 0;
    return 0;
}

int frameworks_validate(const char *framework_name)
{
    if (!framework_name)
    {
        return -1;
    }
    return 0;
}
