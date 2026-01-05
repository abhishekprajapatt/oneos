#include "var_backups.h"

int var_backups_init(void)
{
    return 0;
}

int var_backups_list(var_backups_entry_t *backups, uint32_t *count)
{
    if (!backups || !count)
    {
        return -1;
    }
    return 0;
}

int var_backups_create(const char *name, const char *source_path)
{
    if (!name || !source_path)
    {
        return -1;
    }
    return 0;
}

int var_backups_restore(const char *backup_name, const char *destination)
{
    if (!backup_name || !destination)
    {
        return -1;
    }
    return 0;
}

int var_backups_delete(const char *backup_name)
{
    if (!backup_name)
    {
        return -1;
    }
    return 0;
}

int var_backups_get_state(var_backups_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
