#include "man_man5.h"

int man_man5_init(void)
{
    return 0;
}

int man_man5_add_format(man_man5_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int man_man5_remove_format(uint32_t format_id)
{
    return 0;
}

int man_man5_get_format(const char *file_name, man_man5_entry_t *entry)
{
    if (!file_name || !entry)
    {
        return -1;
    }
    return 0;
}

int man_man5_get_config_files(man_man5_entry_t *results, uint32_t *count)
{
    if (!results || !count)
    {
        return -1;
    }
    return 0;
}
