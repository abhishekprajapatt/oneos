#include "common_files.h"

int common_files_init(void)
{
    return 0;
}

int common_files_register(const char *file_name, const char *file_path)
{
    if (!file_name || !file_path)
    {
        return -1;
    }
    return 0;
}

int common_files_unregister(const char *file_name)
{
    if (!file_name)
    {
        return -1;
    }
    return 0;
}

int common_files_list(char **files, uint32_t max_files)
{
    if (!files || max_files == 0)
    {
        return -1;
    }
    return 0;
}

int common_files_get_path(const char *file_name, char *path, uint32_t max_len)
{
    if (!file_name || !path || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int common_files_verify(const char *file_name)
{
    if (!file_name)
    {
        return -1;
    }
    return 0;
}
