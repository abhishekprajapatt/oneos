#ifndef CMAKE_H
#define CMAKE_H

#include <stdint.h>

typedef struct
{
    const char *source_dir;
    const char *build_dir;
    const char *generator;
    const char *build_type;
    uint32_t parallel_jobs;
} cmake_config_t;

typedef struct
{
    uint32_t total_targets;
    uint32_t built_targets;
    uint32_t failed_targets;
    uint64_t build_time;
} cmake_build_stats_t;

int cmake_init(void);
int cmake_configure(const char *source_dir, const char *build_dir);
int cmake_configure_with_options(cmake_config_t *config);
int cmake_build(const char *build_dir);
int cmake_install(const char *build_dir, const char *install_dir);
int cmake_get_build_stats(cmake_build_stats_t *stats);

#endif
