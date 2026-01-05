#ifndef VAR_LIB_RPM_H
#define VAR_LIB_RPM_H

#include <stdint.h>

typedef struct
{
    uint32_t rpm_id;
    const char *package_name;
    const char *package_version;
    const char *package_arch;
    uint64_t package_size;
} var_lib_rpm_entry_t;

typedef struct
{
    uint32_t total_packages;
    uint32_t installed_packages;
    uint64_t total_size;
} var_lib_rpm_state_t;

int var_lib_rpm_init(void);
int var_lib_rpm_list(var_lib_rpm_entry_t *packages, uint32_t *count);
int var_lib_rpm_get_package(const char *package_name, var_lib_rpm_entry_t *package);
int var_lib_rpm_verify_package(const char *package_name);
int var_lib_rpm_query(const char *package_name, char *query_result, uint32_t *size);
int var_lib_rpm_get_state(var_lib_rpm_state_t *state);

#endif
