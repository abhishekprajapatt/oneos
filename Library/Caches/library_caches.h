#ifndef LIBRARY_CACHES_H
#define LIBRARY_CACHES_H

#include <stdint.h>

typedef struct
{
    uint32_t cache_id;
    const char *cache_name;
    const char *app_name;
    uint64_t cache_size;
    uint64_t max_size;
} library_caches_entry_t;

typedef struct
{
    uint32_t total_caches;
    uint64_t total_cache_size;
    uint64_t available_space;
    uint32_t cache_evictions;
} library_caches_state_t;

int library_caches_init(void);
int library_caches_create(library_caches_entry_t *entry);
int library_caches_delete(uint32_t cache_id);
int library_caches_clear_app_cache(const char *app_name);
int library_caches_resize(uint32_t cache_id, uint64_t new_size);
int library_caches_get_state(library_caches_state_t *state);

#endif
