#ifndef TEXLIVE_H
#define TEXLIVE_H

#include <stdint.h>

typedef struct
{
    const char *package_name;
    const char *repository;
    uint32_t force_install;
    uint32_t update_all;
} texlive_package_options_t;

typedef struct
{
    uint32_t installed_packages;
    uint32_t total_packages;
    uint64_t total_size;
    const char *version;
} texlive_stats_t;

int texlive_init(void);
int texlive_install_package(const char *package_name);
int texlive_install_with_options(texlive_package_options_t *options);
int texlive_remove_package(const char *package_name);
int texlive_update_all_packages(void);
int texlive_get_stats(texlive_stats_t *stats);

#endif
