#ifndef BUILD_PACKAGE_H
#define BUILD_PACKAGE_H

#include <stdint.h>

typedef struct
{
    uint32_t package_id;
    const char *package_name;
    const char *package_version;
    const char *output_path;
    uint32_t package_status;
} build_package_task_t;

typedef struct
{
    uint32_t total_packages;
    uint32_t successful_packages;
    uint32_t failed_packages;
    uint64_t total_package_size;
} build_package_state_t;

int build_package_init(void);
int build_package_create(build_package_task_t *task);
int build_package_get_contents(uint32_t package_id, char *contents, uint32_t *size);
int build_package_verify(uint32_t package_id);
int build_package_get_status(uint32_t package_id, build_package_task_t *task);
int build_package_get_state(build_package_state_t *state);

#endif
