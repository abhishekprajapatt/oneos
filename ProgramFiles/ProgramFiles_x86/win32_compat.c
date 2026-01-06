#include "win32_compat.h"

int win32_compat_init(void)
{
    return 0;
}

int win32_compat_load_library(const char *lib_name)
{
    if (!lib_name)
    {
        return -1;
    }
    return 0;
}

int win32_compat_get_proc_address(const char *lib_name, const char *func_name)
{
    if (!lib_name || !func_name)
    {
        return -1;
    }
    return 0;
}

int win32_compat_call_function(const char *func_pointer, void *args)
{
    if (!func_pointer)
    {
        return -1;
    }
    return 0;
}

int win32_compat_unload_library(const char *lib_name)
{
    if (!lib_name)
    {
        return -1;
    }
    return 0;
}

int win32_compat_translate_error(uint32_t error_code)
{
    if (error_code == 0)
    {
        return -1;
    }
    return 0;
}
