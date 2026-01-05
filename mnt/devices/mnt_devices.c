#include "mnt_devices.h"

int mnt_devices_init(void)
{
    return 0;
}

int mnt_devices_register(mnt_devices_entry_t *device)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}

int mnt_devices_unregister(uint32_t device_id)
{
    return 0;
}

int mnt_devices_get_info(uint32_t device_id, mnt_devices_entry_t *device)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}

int mnt_devices_scan(void)
{
    return 0;
}

int mnt_devices_get_state(mnt_devices_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
