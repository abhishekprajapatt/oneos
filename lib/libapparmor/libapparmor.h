#ifndef LIBAPPARMOR_H
#define LIBAPPARMOR_H

#include <stdint.h>

typedef struct
{
    uint32_t profile_id;
    const char *profile_name;
    uint32_t mode;
} libapparmor_profile_t;

int libapparmor_init(void);
int libapparmor_load_profile(const char *profile_path);
int libapparmor_get_profile(const char *name, libapparmor_profile_t *profile);
int libapparmor_change_profile(const char *profile_name);
int libapparmor_set_mode(const char *profile_name, uint32_t mode);

#endif
