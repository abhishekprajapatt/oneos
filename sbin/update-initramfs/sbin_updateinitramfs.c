#include "sbin_updateinitramfs.h"

int sbin_updateinitramfs_init(void)
{
    return 0;
}

int sbin_updateinitramfs_update(sbin_updateinitramfs_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_updateinitramfs_verify(const char *kernel_version)
{
    if (!kernel_version)
    {
        return -1;
    }
    return 0;
}

int sbin_updateinitramfs_get_status(uint32_t update_id, sbin_updateinitramfs_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_updateinitramfs_get_state(sbin_updateinitramfs_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
