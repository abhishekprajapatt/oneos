#ifndef SETUPTOOLS_H
#define SETUPTOOLS_H

#include <stdint.h>

typedef struct
{
    const char *setup_script;
    const char *command;
    char **options;
    uint32_t option_count;
} setuptools_build_options_t;

typedef struct
{
    uint32_t total_builds;
    uint32_t successful;
    uint32_t failed;
    uint64_t total_time;
} setuptools_stats_t;

int setuptools_init(void);
int setuptools_install(const char *source_dir);
int setuptools_build_distribution(const char *setup_py_dir);
int setuptools_build_with_options(setuptools_build_options_t *options);
int setuptools_create_package(const char *package_dir, const char *output_dir);
int setuptools_get_stats(setuptools_stats_t *stats);

#endif
