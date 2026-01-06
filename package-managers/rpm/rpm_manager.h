#ifndef RPM_MANAGER_H
#define RPM_MANAGER_H

#include <stdint.h>

typedef struct
{
    const char *package_name;
    const char *version;
    const char *release;
    const char *arch;
} rpm_package_t;

int rpm_manager_init(void);
int rpm_manager_install_rpm(const char *rpm_file_path);
int rpm_manager_remove_package(const char *package_name);
int rpm_manager_query_package(const char *package_name, rpm_package_t *pkg);
int rpm_manager_upgrade_package(const char *rpm_file_path);
int rpm_manager_check_dependencies(const char *rpm_file_path);
int rpm_manager_get_installed(rpm_package_t **packages, uint32_t *count);

#endif
