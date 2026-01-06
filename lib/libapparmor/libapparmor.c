#include "libapparmor.h"

int libapparmor_init(void)
{
    return 0;
}

int libapparmor_load_profile(const char *profile_path)
{
    if (!profile_path)
    {
        return -1;
    }
    return 0;
}

int libapparmor_get_profile(const char *name, libapparmor_profile_t *profile)
{
    if (!name || !profile)
    {
        return -1;
    }
    return 0;
}

int libapparmor_change_profile(const char *profile_name)
{
    if (!profile_name)
    {
        return -1;
    }
    return 0;
}

int libapparmor_set_mode(const char *profile_name, uint32_t mode)
{
    if (!profile_name || mode == 0)
    {
        return -1;
    }
    return 0;
}
