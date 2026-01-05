#include "node.h"

int node_init(void)
{
    return 0;
}

int node_execute_script(const char *script_file)
{
    if (!script_file)
    {
        return -1;
    }
    return 0;
}

int node_execute_with_options(node_execution_options_t *options)
{
    if (!options || !options->script_file)
    {
        return -1;
    }
    return 0;
}

int node_install_package(const char *package_name)
{
    if (!package_name)
    {
        return -1;
    }
    return 0;
}

int node_get_runtime_info(node_runtime_info_t *info)
{
    if (!info)
    {
        return -1;
    }
    return 0;
}

int node_repl_start(void)
{
    return 0;
}
