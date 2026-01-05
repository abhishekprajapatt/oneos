#ifndef MM_ALLOCATOR_H
#define MM_ALLOCATOR_H

#include <stdint.h>

typedef struct
{
    uint32_t alloc_id;
    uint64_t address;
    uint64_t size;
    uint32_t flags;
} mm_allocator_block_t;

typedef struct
{
    uint64_t total_allocated;
    uint64_t total_free;
    uint32_t block_count;
    uint32_t fragmentation_percent;
} mm_allocator_state_t;

int mm_allocator_init(uint64_t heap_start, uint64_t heap_size);
int mm_allocator_alloc(uint64_t size, uint32_t flags, uint64_t *address);
int mm_allocator_free(uint64_t address);
int mm_allocator_realloc(uint64_t address, uint64_t new_size, uint64_t *new_address);
int mm_allocator_get_stats(mm_allocator_state_t *state);
int mm_allocator_defragment(void);

#endif
