#include "libffi.h"

int libffi_init(void)
{
    return 0;
}

int libffi_prep_cif(void *cif, uint32_t abi, void *args, void *rtype)
{
    if (!cif || !args || !rtype)
    {
        return -1;
    }
    return 0;
}

int libffi_call(void *cif, void *fn, void *rvalue, void **avalue)
{
    if (!cif || !fn)
    {
        return -1;
    }
    return 0;
}

int libffi_get_type(const char *type_name, libffi_type_t *type)
{
    if (!type_name || !type)
    {
        return -1;
    }
    return 0;
}

int libffi_sizeof_arg(uint32_t type)
{
    if (type == 0)
    {
        return -1;
    }
    return 0;
}
