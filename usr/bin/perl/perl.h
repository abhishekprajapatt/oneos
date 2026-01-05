#ifndef PERL_H
#define PERL_H

#include <stdint.h>

typedef struct
{
    const char *script_file;
    char **arguments;
    uint32_t arg_count;
    uint32_t warnings_enabled;
    uint32_t strict_enabled;
} perl_execution_options_t;

typedef struct
{
    const char *version;
    char *include_paths;
    uint32_t modules_loaded;
} perl_runtime_info_t;

int perl_init(void);
int perl_execute_script(const char *script_file);
int perl_execute_with_options(perl_execution_options_t *options);
int perl_install_module(const char *module_name);
int perl_get_runtime_info(perl_runtime_info_t *info);
int perl_run_one_liner(const char *code);

#endif
