#include "library.h"

int library_init(void)
{
    return 0;
}

int library_load_plugin(const char *plugin_path)
{
    if (!plugin_path)
    {
        return -1;
    }
    return 0;
}

int library_unload_plugin(const char *plugin_name)
{
    if (!plugin_name)
    {
        return -1;
    }
    return 0;
}

int library_get_symbol(const char *plugin_name, const char *symbol, void **ptr)
{
    if (!plugin_name || !symbol || !ptr)
    {
        return -1;
    }
    return 0;
}

int library_list_plugins(char **plugins, uint32_t max_plugins)
{
    if (!plugins || max_plugins == 0)
    {
        return -1;
    }
    return 0;
}
