#include "var.h"

int var_init(void)
{
    return 0;
}

int var_create_directory(const char *path)
{
    if (!path)
    {
        return -1;
    }
    return 0;
}

int var_write_log(const char *component, const char *message)
{
    if (!component || !message)
    {
        return -1;
    }
    return 0;
}

int var_get_log(const char *component, char *buffer, uint32_t max_len)
{
    if (!component || !buffer || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int var_rotate_logs(const char *component)
{
    if (!component)
    {
        return -1;
    }
    return 0;
}
