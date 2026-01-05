#ifndef PIP_H
#define PIP_H

#include <stdint.h>

typedef struct
{
    const char *package_name;
    const char *version;
    uint32_t upgrade;
    uint32_t force_reinstall;
    const char *index_url;
} pip_install_options_t;

typedef struct
{
    uint32_t total_packages;
    uint32_t installed;
    uint32_t outdated;
    uint64_t total_size;
} pip_stats_t;

int pip_init(void);
int pip_install_package(const char *package_name);
int pip_install_with_options(pip_install_options_t *options);
int pip_uninstall_package(const char *package_name);
int pip_search_package(const char *package_name, char *results, uint32_t results_size);
int pip_get_stats(pip_stats_t *stats);

#endif
