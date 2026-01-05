#include "include_linux.h"

int include_linux_get_version(include_linux_version_t *version)
{
    if (!version)
    {
        return -1;
    }
    return 0;
}

int include_linux_get_subsystem(const char *subsystem_name, include_linux_subsystem_t *subsystem)
{
    if (!subsystem_name || !subsystem)
    {
        return -1;
    }
    return 0;
}

int include_linux_register_subsystem(include_linux_subsystem_t *subsystem)
{
    if (!subsystem)
    {
        return -1;
    }
    return 0;
}

int include_linux_unregister_subsystem(uint32_t subsystem_id)
{
    return 0;
}

int include_linux_get_capability(const char *capability_name)
{
    if (!capability_name)
    {
        return -1;
    }
    return 0;
}

int include_linux_request_feature(const char *feature_name)
{
    if (!feature_name)
    {
        return -1;
    }
    return 0;
}
