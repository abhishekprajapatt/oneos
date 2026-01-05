#ifndef VAR_LIB_DPKG_H
#define VAR_LIB_DPKG_H

#include <stdint.h>

typedef struct
{
    uint32_t dpkg_id;
    const char *package_name;
    const char *package_version;
    const char *package_status;
    uint64_t package_size;
} var_lib_dpkg_entry_t;

typedef struct
{
    uint32_t total_packages;
    uint32_t configured_packages;
    uint32_t unpacked_packages;
    uint64_t total_size;
} var_lib_dpkg_state_t;

int var_lib_dpkg_init(void);
int var_lib_dpkg_list(var_lib_dpkg_entry_t *packages, uint32_t *count);
int var_lib_dpkg_get_package(const char *package_name, var_lib_dpkg_entry_t *package);
int var_lib_dpkg_mark_configured(const char *package_name);
int var_lib_dpkg_mark_unconfigured(const char *package_name);
int var_lib_dpkg_get_state(var_lib_dpkg_state_t *state);

#endif
