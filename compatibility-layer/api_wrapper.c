#include "api_wrapper.h"

int api_wrapper_init(void)
{
    return 0;
}

int api_wrapper_register(api_wrapper_config_t *config)
{
    if (!config)
    {
        return -1;
    }
    return 0;
}

int api_wrapper_call(const char *api_name, void *args)
{
    if (!api_name)
    {
        return -1;
    }
    return 0;
}

int api_wrapper_translate_error(int error_code)
{
    return error_code;
}

int api_wrapper_get_version_mapping(const char *api_name, char *version, uint32_t max_len)
{
    if (!api_name || !version || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int api_wrapper_validate_compatibility(const char *api_name)
{
    if (!api_name)
    {
        return -1;
    }
    return 0;
}
