#ifndef RUBY_H
#define RUBY_H

#include <stdint.h>

typedef struct
{
    const char *script_file;
    char **arguments;
    uint32_t arg_count;
    uint32_t verbose;
    uint32_t jit_enabled;
} ruby_execution_options_t;

typedef struct
{
    const char *ruby_version;
    const char *patch_level;
    uint32_t gc_stats_enabled;
    uint64_t memory_allocated;
} ruby_runtime_info_t;

int ruby_init(void);
int ruby_execute_script(const char *script_file);
int ruby_execute_with_options(ruby_execution_options_t *options);
int ruby_run_code(const char *code);
int ruby_interactive_shell(void);
int ruby_get_runtime_info(ruby_runtime_info_t *info);

#endif
