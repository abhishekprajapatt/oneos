#include "var_tmp.h"

int var_tmp_init(void)
{
    return 0;
}

int var_tmp_list(var_tmp_entry_t *files, uint32_t *count)
{
    if (!files || !count)
    {
        return -1;
    }
    return 0;
}

int var_tmp_create(const char *filename, const void *data, uint64_t size)
{
    if (!filename || !data)
    {
        return -1;
    }
    return 0;
}

int var_tmp_get(const char *filename, var_tmp_entry_t *file)
{
    if (!filename || !file)
    {
        return -1;
    }
    return 0;
}

int var_tmp_delete(const char *filename)
{
    if (!filename)
    {
        return -1;
    }
    return 0;
}

int var_tmp_cleanup_expired(void)
{
    return 0;
}

int var_tmp_get_state(var_tmp_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
