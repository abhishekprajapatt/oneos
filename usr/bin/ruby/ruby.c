#include "ruby.h"

int ruby_init(void)
{
    return 0;
}

int ruby_execute_script(const char *script_file)
{
    if (!script_file)
    {
        return -1;
    }
    return 0;
}

int ruby_execute_with_options(ruby_execution_options_t *options)
{
    if (!options || !options->script_file)
    {
        return -1;
    }
    return 0;
}

int ruby_run_code(const char *code)
{
    if (!code)
    {
        return -1;
    }
    return 0;
}

int ruby_interactive_shell(void)
{
    return 0;
}

int ruby_get_runtime_info(ruby_runtime_info_t *info)
{
    if (!info)
    {
        return -1;
    }
    return 0;
}
