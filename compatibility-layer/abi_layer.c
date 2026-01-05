#include "abi_layer.h"

int abi_init(void)
{
    return 0;
}

int abi_register_interface(abi_info_t *abi)
{
    if (!abi)
    {
        return -1;
    }
    return 0;
}

int abi_check_compatibility(abi_info_t *abi1, abi_info_t *abi2)
{
    if (!abi1 || !abi2)
    {
        return -1;
    }
    return 0;
}

int abi_translate_structure(const char *struct_name, void *src, void *dst)
{
    if (!struct_name || !src || !dst)
    {
        return -1;
    }
    return 0;
}

int abi_get_calling_convention(char *convention, uint32_t max_len)
{
    if (!convention || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int abi_validate_binary(const char *binary_path)
{
    if (!binary_path)
    {
        return -1;
    }
    return 0;
}
