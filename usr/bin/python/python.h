#ifndef PYTHON_H
#define PYTHON_H

#include <stdint.h>

typedef struct
{
    const char *script_file;
    char **arguments;
    uint32_t arg_count;
    uint32_t optimization_level;
    const char *python_path;
} python_execution_options_t;

typedef struct
{
    const char *python_version;
    const char *prefix;
    uint32_t interactive_mode;
    uint64_t memory_usage;
} python_runtime_info_t;

int python_init(void);
int python_execute_script(const char *script_file);
int python_execute_with_options(python_execution_options_t *options);
int python_run_code(const char *code);
int python_interactive_shell(void);
int python_get_runtime_info(python_runtime_info_t *info);

#endif
