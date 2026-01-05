#include "php.h"

int php_init(void)
{
    return 0;
}

int php_execute_script(const char *script_file)
{
    if (!script_file)
    {
        return -1;
    }
    return 0;
}

int php_execute_with_options(php_execution_options_t *options)
{
    if (!options || !options->script_file)
    {
        return -1;
    }
    return 0;
}

int php_run_code(const char *php_code)
{
    if (!php_code)
    {
        return -1;
    }
    return 0;
}

int php_get_runtime_info(php_runtime_info_t *info)
{
    if (!info)
    {
        return -1;
    }
    return 0;
}

int php_install_extension(const char *extension_name)
{
    if (!extension_name)
    {
        return -1;
    }
    return 0;
}
