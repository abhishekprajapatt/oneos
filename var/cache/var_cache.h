#ifndef VAR_CACHE_H
#define VAR_CACHE_H

#include <stdint.h>

typedef struct
{
    uint32_t cache_id;
    const char *cache_name;
    const char *cache_type;
    uint64_t cache_size;
    uint64_t last_accessed;
} var_cache_entry_t;

typedef struct
{
    uint32_t total_cache_entries;
    uint64_t total_cache_size;
    uint32_t evicted_entries;
} var_cache_state_t;

int var_cache_init(void);
int var_cache_list(var_cache_entry_t *entries, uint32_t *count);
int var_cache_get(const char *cache_name, var_cache_entry_t *entry);
int var_cache_store(const char *name, const char *type, const void *data, uint64_t size);
int var_cache_evict(const char *cache_name);
int var_cache_clear(void);
int var_cache_get_state(var_cache_state_t *state);

#endif
