#ifndef BUILD_SYSTEM_H
#define BUILD_SYSTEM_H

#include <stdint.h>

int build_system_init(void);
int build_system_compile(const char *source_file, const char *output_file);
int build_system_link(const char **object_files, uint32_t count, const char *output);
int build_system_clean(const char *directory);
int build_system_parallel_build(uint32_t num_jobs);
int build_system_get_status(uint32_t *progress);

#endif
