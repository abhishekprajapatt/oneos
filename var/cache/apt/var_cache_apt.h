#ifndef VAR_CACHE_APT_H
#define VAR_CACHE_APT_H

#include <stdint.h>

typedef struct
{
    uint32_t apt_id;
    const char *package_name;
    const char *package_version;
    uint64_t cache_size;
    uint64_t last_accessed;
} var_cache_apt_entry_t;

typedef struct
{
    uint32_t total_cached_packages;
    uint32_t available_packages;
    uint64_t total_cache_size;
} var_cache_apt_state_t;

int var_cache_apt_init(void);
int var_cache_apt_list(var_cache_apt_entry_t *entries, uint32_t *count);
int var_cache_apt_get(const char *package_name, var_cache_apt_entry_t *entry);
int var_cache_apt_store(const char *package_name, const char *version, const void *data, uint64_t size);
int var_cache_apt_evict(const char *package_name);
int var_cache_apt_clear(void);
int var_cache_apt_get_state(var_cache_apt_state_t *state);

#endif
