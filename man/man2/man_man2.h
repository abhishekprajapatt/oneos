#ifndef MAN_MAN2_H
#define MAN_MAN2_H

#include <stdint.h>

typedef struct
{
    uint32_t syscall_id;
    const char *syscall_name;
    const char *prototype;
    const char *description;
    int32_t return_value;
} man_man2_entry_t;

typedef struct
{
    uint32_t total_syscalls;
    uint32_t documented_syscalls;
    uint32_t syscall_groups;
} man_man2_state_t;

int man_man2_init(void);
int man_man2_add_syscall(man_man2_entry_t *entry);
int man_man2_remove_syscall(uint32_t syscall_id);
int man_man2_get_syscall(const char *name, man_man2_entry_t *entry);
int man_man2_get_syscall_info(uint32_t syscall_id, man_man2_entry_t *entry);

#endif
