#ifndef MM_SLUB_H
#define MM_SLUB_H

#include <stdint.h>

typedef struct
{
    uint32_t cache_id;
    const char *cache_name;
    uint32_t object_size;
    uint32_t objects_per_slab;
    uint32_t slab_count;
} mm_slub_cache_t;

typedef struct
{
    uint32_t cache_count;
    uint64_t total_allocated;
    uint64_t total_freed;
    uint32_t active_objects;
} mm_slub_state_t;

int mm_slub_init(void);
int mm_slub_create_cache(const char *name, uint32_t object_size, mm_slub_cache_t *cache);
int mm_slub_destroy_cache(uint32_t cache_id);
int mm_slub_alloc(uint32_t cache_id, uint64_t *address);
int mm_slub_free(uint32_t cache_id, uint64_t address);
int mm_slub_shrink_cache(uint32_t cache_id);

#endif
