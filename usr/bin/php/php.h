#ifndef PHP_H
#define PHP_H

#include <stdint.h>

typedef struct
{
    const char *script_file;
    char **arguments;
    uint32_t arg_count;
    uint32_t run_cli;
    const char *config_file;
} php_execution_options_t;

typedef struct
{
    const char *php_version;
    const char *sapi;
    char *loaded_extensions;
    uint32_t extension_count;
} php_runtime_info_t;

int php_init(void);
int php_execute_script(const char *script_file);
int php_execute_with_options(php_execution_options_t *options);
int php_run_code(const char *php_code);
int php_get_runtime_info(php_runtime_info_t *info);
int php_install_extension(const char *extension_name);

#endif
