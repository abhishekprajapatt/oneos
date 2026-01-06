#include "extensions.h"

int extensions_init(void)
{
    return 0;
}

int extensions_load(const char *extension_path)
{
    if (!extension_path)
    {
        return -1;
    }
    return 0;
}

int extensions_unload(const char *extension_name)
{
    if (!extension_name)
    {
        return -1;
    }
    return 0;
}

int extensions_list(extension_t **extensions, uint32_t *count)
{
    if (!extensions || !count)
    {
        return -1;
    }
    *count = 0;
    return 0;
}

int extensions_enable(const char *extension_name)
{
    if (!extension_name)
    {
        return -1;
    }
    return 0;
}

int extensions_disable(const char *extension_name)
{
    if (!extension_name)
    {
        return -1;
    }
    return 0;
}
