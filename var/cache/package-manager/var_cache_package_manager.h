#ifndef VAR_CACHE_PACKAGE_MANAGER_H
#define VAR_CACHE_PACKAGE_MANAGER_H

#include <stdint.h>

typedef struct
{
    uint32_t pkg_id;
    const char *package_name;
    const char *package_version;
    const char *manager_type;
    uint64_t cache_size;
} var_cache_package_manager_entry_t;

typedef struct
{
    uint32_t total_packages;
    uint32_t cached_packages;
    uint64_t total_cache_size;
} var_cache_package_manager_state_t;

int var_cache_package_manager_init(void);
int var_cache_package_manager_list(var_cache_package_manager_entry_t *entries, uint32_t *count);
int var_cache_package_manager_get(const char *package_name, var_cache_package_manager_entry_t *entry);
int var_cache_package_manager_cache_package(const char *package_name, const char *version, const char *manager);
int var_cache_package_manager_evict(const char *package_name);
int var_cache_package_manager_clear(void);
int var_cache_package_manager_get_state(var_cache_package_manager_state_t *state);

#endif
