#ifndef MAKE_H
#define MAKE_H

#include <stdint.h>

typedef struct
{
    const char *makefile_path;
    const char *target;
    uint32_t parallel_jobs;
    char **variables;
    uint32_t variable_count;
} make_build_options_t;

typedef struct
{
    uint32_t total_targets;
    uint32_t built_targets;
    uint32_t failed_targets;
    uint64_t build_duration;
} make_build_stats_t;

int make_init(void);
int make_build(const char *makefile_path);
int make_build_target(const char *makefile_path, const char *target);
int make_build_with_options(make_build_options_t *options);
int make_get_targets(const char *makefile_path, char *targets, uint32_t targets_size);
int make_get_build_stats(make_build_stats_t *stats);

#endif
