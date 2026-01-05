#include "sys_block.h"

int sys_block_init(void)
{
    return 0;
}

int sys_block_register_device(sys_block_device_t *device)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}

int sys_block_unregister_device(uint32_t device_id)
{
    return 0;
}

int sys_block_read_block(uint32_t device_id, uint64_t block_num, void *buffer)
{
    if (!buffer)
    {
        return -1;
    }
    return 0;
}

int sys_block_write_block(uint32_t device_id, uint64_t block_num, const void *buffer)
{
    if (!buffer)
    {
        return -1;
    }
    return 0;
}

int sys_block_get_device_info(uint32_t device_id, sys_block_device_t *device)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}

int sys_block_get_state(sys_block_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
