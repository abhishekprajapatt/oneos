#include "app_info.h"

int info_init(void)
{
    return 0;
}

int info_load_plist(const char *plist_path, info_plist_t *info)
{
    if (!plist_path || !info)
    {
        return -1;
    }
    return 0;
}

int info_get_version(char *version, uint32_t max_len)
{
    if (!version || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int info_get_bundle_identifier(char *identifier, uint32_t max_len)
{
    if (!identifier || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int info_validate_bundle(void)
{
    return 0;
}

int info_get_executable_path(char *path, uint32_t max_len)
{
    if (!path || max_len == 0)
    {
        return -1;
    }
    return 0;
}
