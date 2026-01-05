#include "include_uapi.h"

int include_uapi_get_version(include_uapi_version_t *version)
{
    if (!version)
    {
        return -1;
    }
    return 0;
}

int include_uapi_register_ioctl(include_uapi_ioctl_t *ioctl)
{
    if (!ioctl)
    {
        return -1;
    }
    return 0;
}

int include_uapi_unregister_ioctl(uint32_t ioctl_id)
{
    return 0;
}

int include_uapi_invoke_ioctl(uint32_t ioctl_id, void *data)
{
    return 0;
}

int include_uapi_get_feature(const char *feature_name)
{
    if (!feature_name)
    {
        return -1;
    }
    return 0;
}

int include_uapi_enable_feature(const char *feature_name)
{
    if (!feature_name)
    {
        return -1;
    }
    return 0;
}
