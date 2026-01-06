#include "libglib.h"

int libglib_init(void)
{
    return 0;
}

int libglib_g_malloc(uint32_t size, void **ptr)
{
    if (size == 0 || !ptr)
    {
        return -1;
    }
    return 0;
}

int libglib_g_free(void *ptr)
{
    if (!ptr)
    {
        return -1;
    }
    return 0;
}

int libglib_g_list_append(libglib_list_node_t *list, void *data)
{
    if (!list)
    {
        return -1;
    }
    return 0;
}

int libglib_g_string_new(const char *str, void **g_string)
{
    if (!str || !g_string)
    {
        return -1;
    }
    return 0;
}

int libglib_g_hash_table_new(void **hash_table)
{
    if (!hash_table)
    {
        return -1;
    }
    return 0;
}
