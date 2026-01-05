#include "mm_allocator.h"

int mm_allocator_init(uint64_t heap_start, uint64_t heap_size)
{
    return 0;
}

int mm_allocator_alloc(uint64_t size, uint32_t flags, uint64_t *address)
{
    if (!address)
    {
        return -1;
    }
    return 0;
}

int mm_allocator_free(uint64_t address)
{
    return 0;
}

int mm_allocator_realloc(uint64_t address, uint64_t new_size, uint64_t *new_address)
{
    if (!new_address)
    {
        return -1;
    }
    return 0;
}

int mm_allocator_get_stats(mm_allocator_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}

int mm_allocator_defragment(void)
{
    return 0;
}
