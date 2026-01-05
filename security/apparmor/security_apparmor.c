#include "security_apparmor.h"

int security_apparmor_init(void)
{
    return 0;
}

int security_apparmor_load_profile(security_apparmor_profile_t *profile)
{
    if (!profile)
    {
        return -1;
    }
    return 0;
}

int security_apparmor_unload_profile(uint32_t profile_id)
{
    return 0;
}

int security_apparmor_set_mode(uint32_t profile_id, uint32_t mode)
{
    return 0;
}

int security_apparmor_get_profile(uint32_t profile_id, security_apparmor_profile_t *profile)
{
    if (!profile)
    {
        return -1;
    }
    return 0;
}

int security_apparmor_get_state(security_apparmor_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
