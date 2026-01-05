#ifndef VAR_LIB_APT_H
#define VAR_LIB_APT_H

#include <stdint.h>

typedef struct
{
    uint32_t apt_id;
    const char *package_name;
    const char *package_status;
    const char *package_path;
    uint64_t package_size;
} var_lib_apt_entry_t;

typedef struct
{
    uint32_t total_packages;
    uint32_t installed_packages;
    uint32_t pending_packages;
    uint64_t total_size;
} var_lib_apt_state_t;

int var_lib_apt_init(void);
int var_lib_apt_list(var_lib_apt_entry_t *packages, uint32_t *count);
int var_lib_apt_get_package(const char *package_name, var_lib_apt_entry_t *package);
int var_lib_apt_update_package(const char *package_name, const char *status);
int var_lib_apt_remove_package(const char *package_name);
int var_lib_apt_get_state(var_lib_apt_state_t *state);

#endif
