#ifndef MM_SWAP_H
#define MM_SWAP_H

#include <stdint.h>

typedef struct
{
    uint32_t swap_id;
    const char *swap_device;
    uint64_t swap_size;
    uint64_t used_space;
    uint32_t priority;
} mm_swap_device_t;

typedef struct
{
    uint32_t device_count;
    uint64_t total_swap;
    uint64_t used_swap;
    uint64_t swap_in_count;
    uint64_t swap_out_count;
} mm_swap_state_t;

int mm_swap_init(void);
int mm_swap_register_device(mm_swap_device_t *device);
int mm_swap_unregister_device(uint32_t swap_id);
int mm_swap_in_page(uint64_t swap_offset, uint64_t *physical_addr);
int mm_swap_out_page(uint64_t physical_addr, uint64_t *swap_offset);
int mm_swap_get_stats(mm_swap_state_t *state);

#endif
