#include "mm_swap.h"

int mm_swap_init(void)
{
    return 0;
}

int mm_swap_register_device(mm_swap_device_t *device)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}

int mm_swap_unregister_device(uint32_t swap_id)
{
    return 0;
}

int mm_swap_in_page(uint64_t swap_offset, uint64_t *physical_addr)
{
    if (!physical_addr)
    {
        return -1;
    }
    return 0;
}

int mm_swap_out_page(uint64_t physical_addr, uint64_t *swap_offset)
{
    if (!swap_offset)
    {
        return -1;
    }
    return 0;
}

int mm_swap_get_stats(mm_swap_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
