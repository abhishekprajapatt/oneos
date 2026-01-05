#include "include_sys.h"

int include_sys_syscall(include_sys_syscall_t *syscall, include_sys_result_t *result)
{
    if (!syscall || !result)
    {
        return -1;
    }
    return 0;
}

int include_sys_get_errno(uint32_t *errno_val)
{
    if (!errno_val)
    {
        return -1;
    }
    return 0;
}

int include_sys_set_errno(uint32_t errno_val)
{
    return 0;
}

int include_sys_get_syscall_info(uint32_t syscall_id, char *info, uint32_t size)
{
    if (!info)
    {
        return -1;
    }
    return 0;
}

int include_sys_register_syscall(uint32_t syscall_id, void *handler)
{
    if (!handler)
    {
        return -1;
    }
    return 0;
}

int include_sys_unregister_syscall(uint32_t syscall_id)
{
    return 0;
}
