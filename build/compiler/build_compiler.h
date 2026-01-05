#ifndef BUILD_COMPILER_H
#define BUILD_COMPILER_H

#include <stdint.h>

typedef struct
{
    uint32_t compile_id;
    const char *source_file;
    const char *output_file;
    uint32_t compile_status;
    uint32_t error_count;
} build_compiler_task_t;

typedef struct
{
    uint32_t total_compiles;
    uint32_t successful_compiles;
    uint32_t failed_compiles;
    uint64_t total_lines_compiled;
} build_compiler_state_t;

int build_compiler_init(void);
int build_compiler_compile(build_compiler_task_t *task);
int build_compiler_get_errors(uint32_t compile_id, char *errors, uint32_t *size);
int build_compiler_cancel(uint32_t compile_id);
int build_compiler_get_status(uint32_t compile_id, build_compiler_task_t *task);
int build_compiler_get_state(build_compiler_state_t *state);

#endif
