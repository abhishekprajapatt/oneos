#include "sbin_mkfs.h"

int sbin_mkfs_init(void)
{
    return 0;
}

int sbin_mkfs_create(sbin_mkfs_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_mkfs_verify(const char *device)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}

int sbin_mkfs_get_status(uint32_t mkfs_id, sbin_mkfs_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_mkfs_get_state(sbin_mkfs_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
