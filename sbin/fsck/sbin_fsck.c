#include "sbin_fsck.h"

int sbin_fsck_init(void)
{
    return 0;
}

int sbin_fsck_check_filesystem(sbin_fsck_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_fsck_repair(const char *device, uint32_t flags)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}

int sbin_fsck_get_result(uint32_t check_id, sbin_fsck_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_fsck_get_state(sbin_fsck_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
