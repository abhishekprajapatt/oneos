#ifndef VAR_CACHE_MAN_H
#define VAR_CACHE_MAN_H

#include <stdint.h>

typedef struct
{
    uint32_t man_id;
    const char *manual_name;
    const char *manual_section;
    uint64_t cache_size;
    uint64_t last_accessed;
} var_cache_man_entry_t;

typedef struct
{
    uint32_t total_manuals;
    uint32_t cached_manuals;
    uint64_t total_cache_size;
} var_cache_man_state_t;

int var_cache_man_init(void);
int var_cache_man_list(var_cache_man_entry_t *entries, uint32_t *count);
int var_cache_man_get(const char *manual_name, var_cache_man_entry_t *entry);
int var_cache_man_cache_manual(const char *manual_name, const char *section);
int var_cache_man_invalidate(const char *manual_name);
int var_cache_man_rebuild(void);
int var_cache_man_get_state(var_cache_man_state_t *state);

#endif
