#ifndef SECURITY_APPARMOR_H
#define SECURITY_APPARMOR_H

#include <stdint.h>

typedef struct
{
    uint32_t profile_id;
    const char *profile_name;
    const char *profile_path;
    uint32_t profile_mode;
    uint32_t profile_status;
} security_apparmor_profile_t;

typedef struct
{
    uint32_t total_profiles;
    uint32_t loaded_profiles;
    uint32_t enforce_mode;
    uint32_t complain_mode;
} security_apparmor_state_t;

int security_apparmor_init(void);
int security_apparmor_load_profile(security_apparmor_profile_t *profile);
int security_apparmor_unload_profile(uint32_t profile_id);
int security_apparmor_set_mode(uint32_t profile_id, uint32_t mode);
int security_apparmor_get_profile(uint32_t profile_id, security_apparmor_profile_t *profile);
int security_apparmor_get_state(security_apparmor_state_t *state);

#endif
