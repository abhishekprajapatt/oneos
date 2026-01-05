#include "man_man8.h"

int man_man8_init(void)
{
    return 0;
}

int man_man8_add_command(man_man8_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int man_man8_remove_command(uint32_t admin_cmd_id)
{
    return 0;
}

int man_man8_get_command(const char *name, man_man8_entry_t *entry)
{
    if (!name || !entry)
    {
        return -1;
    }
    return 0;
}

int man_man8_get_privileged_commands(uint32_t min_level, man_man8_entry_t *results, uint32_t *count)
{
    if (!results || !count)
    {
        return -1;
    }
    return 0;
}
