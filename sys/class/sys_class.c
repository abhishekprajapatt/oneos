#include "sys_class.h"

int sys_class_init(void)
{
    return 0;
}

int sys_class_register(sys_class_entry_t *class_entry)
{
    if (!class_entry)
    {
        return -1;
    }
    return 0;
}

int sys_class_unregister(uint32_t class_id)
{
    return 0;
}

int sys_class_add_device(uint32_t class_id, uint32_t device_id)
{
    return 0;
}

int sys_class_remove_device(uint32_t class_id, uint32_t device_id)
{
    return 0;
}

int sys_class_get_devices(uint32_t class_id, uint32_t *device_ids, uint32_t *count)
{
    if (!device_ids || !count)
    {
        return -1;
    }
    return 0;
}

int sys_class_get_state(sys_class_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
