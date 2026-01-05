#include "mnt_external.h"

int mnt_external_init(void)
{
    return 0;
}

int mnt_external_connect(mnt_external_device_t *device)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}

int mnt_external_disconnect(uint32_t external_id)
{
    return 0;
}

int mnt_external_mount(uint32_t external_id)
{
    return 0;
}

int mnt_external_unmount(uint32_t external_id)
{
    return 0;
}

int mnt_external_get_state(mnt_external_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
