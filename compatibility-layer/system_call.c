#include "system_call.h"

int syscall_init(void)
{
    return 0;
}

int syscall_register(syscall_info_t *info)
{
    if (!info)
    {
        return -1;
    }
    return 0;
}

int syscall_unregister(uint32_t syscall_id)
{
    if (syscall_id == 0)
    {
        return -1;
    }
    return 0;
}

int syscall_invoke(uint32_t syscall_number, void *args)
{
    if (syscall_number == 0)
    {
        return -1;
    }
    return 0;
}

int syscall_get_mapping(uint32_t original_id, uint32_t *mapped_id)
{
    if (original_id == 0 || !mapped_id)
    {
        return -1;
    }
    return 0;
}

int syscall_validate_args(uint32_t syscall_number, void *args)
{
    if (syscall_number == 0)
    {
        return -1;
    }
    return 0;
}
