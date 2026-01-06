#ifndef SPACE_MANAGER_H
#define SPACE_MANAGER_H

#include <stdint.h>

typedef struct
{
    const char *path;
    uint64_t size;
    const char *category;
} space_usage_t;

int space_manager_init(void);
int space_manager_analyze_usage(const char *volume_path);
int space_manager_get_usage_by_category(space_usage_t **usage, uint32_t *count);
int space_manager_get_largest_files(char **files, uint32_t max_files);
int space_manager_estimate_cleanup(uint64_t *estimated_space);
int space_manager_cleanup_cache(const char *volume_path);
int space_manager_remove_duplicates(const char *volume_path);

#endif
