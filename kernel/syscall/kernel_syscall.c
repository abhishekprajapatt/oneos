#include "kernel_syscall.h"

int kernel_syscall_init(void)
{
    return 0;
}

int kernel_syscall_register(uint32_t syscall_num, void *handler)
{
    if (!handler)
    {
        return -1;
    }
    return 0;
}

int kernel_syscall_unregister(uint32_t syscall_num)
{
    return 0;
}

int kernel_syscall_invoke(kernel_syscall_request_t *request, kernel_syscall_result_t *result)
{
    if (!request || !result)
    {
        return -1;
    }
    return 0;
}

int kernel_syscall_get_handler(uint32_t syscall_num, void **handler)
{
    if (!handler)
    {
        return -1;
    }
    return 0;
}

int kernel_syscall_trace(uint32_t syscall_num, uint8_t enable)
{
    return 0;
}
