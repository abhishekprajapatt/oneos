#ifndef SYSTEM_CALL_H
#define SYSTEM_CALL_H

#include <stdint.h>

typedef struct
{
    uint32_t syscall_id;
    const char *syscall_name;
    uint32_t arg_count;
} syscall_info_t;

int syscall_init(void);
int syscall_register(syscall_info_t *info);
int syscall_unregister(uint32_t syscall_id);
int syscall_invoke(uint32_t syscall_number, void *args);
int syscall_get_mapping(uint32_t original_id, uint32_t *mapped_id);
int syscall_validate_args(uint32_t syscall_number, void *args);

#endif
