#ifndef EASY_INSTALL_H
#define EASY_INSTALL_H

#include <stdint.h>

typedef struct
{
    const char *package_name;
    const char *version;
    const char *repository;
    uint32_t force_reinstall;
    uint32_t upgrade;
} easy_install_options_t;

typedef struct
{
    uint32_t total_installed;
    uint32_t successfully_installed;
    uint32_t failed_installs;
    uint64_t total_size;
} easy_install_stats_t;

int easy_install_init(void);
int easy_install_package(const char *package_name);
int easy_install_with_options(easy_install_options_t *options);
int easy_install_search(const char *package_name, char *results, uint32_t results_size);
int easy_install_remove(const char *package_name);
int easy_install_get_stats(easy_install_stats_t *stats);

#endif
