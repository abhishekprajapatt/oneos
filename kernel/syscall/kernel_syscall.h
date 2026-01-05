#ifndef KERNEL_SYSCALL_H
#define KERNEL_SYSCALL_H

#include <stdint.h>

typedef struct
{
    uint32_t syscall_num;
    uint32_t arg_count;
    uint64_t args[6];
} kernel_syscall_request_t;

typedef struct
{
    int64_t return_value;
    uint32_t errno_val;
} kernel_syscall_result_t;

int kernel_syscall_init(void);
int kernel_syscall_register(uint32_t syscall_num, void *handler);
int kernel_syscall_unregister(uint32_t syscall_num);
int kernel_syscall_invoke(kernel_syscall_request_t *request, kernel_syscall_result_t *result);
int kernel_syscall_get_handler(uint32_t syscall_num, void **handler);
int kernel_syscall_trace(uint32_t syscall_num, uint8_t enable);

#endif
