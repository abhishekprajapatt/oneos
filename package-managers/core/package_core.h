#ifndef PACKAGE_CORE_H
#define PACKAGE_CORE_H

#include <stdint.h>

typedef struct
{
    const char *package_name;
    const char *version;
    const char *maintainer;
    uint32_t install_size;
} package_info_t;

int package_core_init(void);
int package_core_register_manager(const char *manager_type);
int package_core_detect_system(char *system_type, uint32_t max_len);
int package_core_get_manager(const char *manager_type);
int package_core_validate_package(const char *package_file);
int package_core_extract_metadata(const char *package_file, package_info_t *info);

#endif
