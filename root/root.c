#include "root.h"

int root_init(void)
{
    return 0;
}

int root_create_file(const char *path)
{
    if (!path)
    {
        return -1;
    }
    return 0;
}

int root_delete_file(const char *path)
{
    if (!path)
    {
        return -1;
    }
    return 0;
}

int root_create_directory(const char *path)
{
    if (!path)
    {
        return -1;
    }
    return 0;
}

int root_delete_directory(const char *path)
{
    if (!path)
    {
        return -1;
    }
    return 0;
}

int root_get_current_dir(char *path, uint32_t max_len)
{
    if (!path || max_len == 0)
    {
        return -1;
    }
    return 0;
}
