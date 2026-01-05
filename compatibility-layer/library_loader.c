#include "library_loader.h"

int loader_init(void)
{
    return 0;
}

int loader_load_library(const char *lib_name, library_info_t *info)
{
    if (!lib_name || !info)
    {
        return -1;
    }
    return 0;
}

int loader_unload_library(uint32_t lib_id)
{
    if (lib_id == 0)
    {
        return -1;
    }
    return 0;
}

int loader_resolve_symbol(uint32_t lib_id, const char *symbol, void **symbol_ptr)
{
    if (lib_id == 0 || !symbol || !symbol_ptr)
    {
        return -1;
    }
    return 0;
}

int loader_get_library_version(const char *lib_name, char *version, uint32_t max_len)
{
    if (!lib_name || !version || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int loader_validate_abi(uint32_t lib_id)
{
    if (lib_id == 0)
    {
        return -1;
    }
    return 0;
}
