#include "man_man2.h"

int man_man2_init(void)
{
    return 0;
}

int man_man2_add_syscall(man_man2_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int man_man2_remove_syscall(uint32_t syscall_id)
{
    return 0;
}

int man_man2_get_syscall(const char *name, man_man2_entry_t *entry)
{
    if (!name || !entry)
    {
        return -1;
    }
    return 0;
}

int man_man2_get_syscall_info(uint32_t syscall_id, man_man2_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}
