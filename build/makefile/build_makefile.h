#ifndef BUILD_MAKEFILE_H
#define BUILD_MAKEFILE_H

#include <stdint.h>

typedef struct
{
    uint32_t build_id;
    const char *makefile_path;
    const char *build_target;
    uint32_t build_status;
    uint32_t build_time;
} build_makefile_task_t;

typedef struct
{
    uint32_t total_builds;
    uint32_t successful_builds;
    uint32_t failed_builds;
    uint64_t total_build_time;
} build_makefile_state_t;

int build_makefile_init(void);
int build_makefile_execute(build_makefile_task_t *task);
int build_makefile_clean(const char *makefile_path);
int build_makefile_get_targets(const char *makefile_path, char **targets, uint32_t *count);
int build_makefile_get_status(uint32_t build_id, build_makefile_task_t *task);
int build_makefile_get_state(build_makefile_state_t *state);

#endif
