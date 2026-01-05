#include "sys_kernel.h"

int sys_kernel_init(void)
{
    return 0;
}

int sys_kernel_load_module(sys_kernel_module_t *module)
{
    if (!module)
    {
        return -1;
    }
    return 0;
}

int sys_kernel_unload_module(uint32_t module_id)
{
    return 0;
}

int sys_kernel_get_module(uint32_t module_id, sys_kernel_module_t *module)
{
    if (!module)
    {
        return -1;
    }
    return 0;
}

int sys_kernel_list_modules(sys_kernel_module_t *modules, uint32_t *count)
{
    if (!modules || !count)
    {
        return -1;
    }
    return 0;
}

int sys_kernel_get_state(sys_kernel_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
