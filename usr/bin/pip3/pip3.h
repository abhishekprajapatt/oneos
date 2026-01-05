#ifndef PIP3_H
#define PIP3_H

#include <stdint.h>

typedef struct
{
    const char *package_name;
    const char *version;
    uint32_t upgrade;
    uint32_t user_install;
    const char *requirements_file;
} pip3_install_options_t;

typedef struct
{
    uint32_t total_packages;
    uint32_t installed;
    uint32_t outdated;
    uint64_t total_size;
} pip3_stats_t;

int pip3_init(void);
int pip3_install_package(const char *package_name);
int pip3_install_with_options(pip3_install_options_t *options);
int pip3_uninstall_package(const char *package_name);
int pip3_list_packages(char *package_list, uint32_t list_size);
int pip3_get_stats(pip3_stats_t *stats);

#endif
