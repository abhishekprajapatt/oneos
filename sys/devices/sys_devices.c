#include "sys_devices.h"

int sys_devices_init(void)
{
    return 0;
}

int sys_devices_register(sys_devices_entry_t *device)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}

int sys_devices_unregister(uint32_t device_id)
{
    return 0;
}

int sys_devices_get_device(uint32_t device_id, sys_devices_entry_t *device)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}

int sys_devices_enumerate(sys_devices_entry_t *devices, uint32_t *count)
{
    if (!devices || !count)
    {
        return -1;
    }
    return 0;
}

int sys_devices_get_state(sys_devices_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
