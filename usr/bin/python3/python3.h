#ifndef PYTHON3_H
#define PYTHON3_H

#include <stdint.h>

typedef struct
{
    const char *script_file;
    char **arguments;
    uint32_t arg_count;
    uint32_t dont_write_bytecode;
    const char *home_path;
} python3_execution_options_t;

typedef struct
{
    const char *python_version;
    const char *api_version;
    uint32_t has_debug;
    uint64_t total_memory;
} python3_runtime_info_t;

int python3_init(void);
int python3_execute_script(const char *script_file);
int python3_execute_with_options(python3_execution_options_t *options);
int python3_run_code(const char *code);
int python3_interactive_shell(void);
int python3_get_runtime_info(python3_runtime_info_t *info);

#endif
