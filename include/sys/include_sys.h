#ifndef INCLUDE_SYS_H
#define INCLUDE_SYS_H

#include <stdint.h>

typedef struct
{
    uint32_t syscall_id;
    uint32_t arg_count;
    uint64_t args[6];
} include_sys_syscall_t;

typedef struct
{
    uint64_t ret_value;
    uint32_t errno_val;
} include_sys_result_t;

int include_sys_syscall(include_sys_syscall_t *syscall, include_sys_result_t *result);
int include_sys_get_errno(uint32_t *errno_val);
int include_sys_set_errno(uint32_t errno_val);
int include_sys_get_syscall_info(uint32_t syscall_id, char *info, uint32_t size);
int include_sys_register_syscall(uint32_t syscall_id, void *handler);
int include_sys_unregister_syscall(uint32_t syscall_id);

#endif
