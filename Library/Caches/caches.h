#ifndef CACHES_H
#define CACHES_H

#include <stdint.h>

int caches_init(void);
int caches_get_cache_dir(const char *app_name, char *path, uint32_t max_len);
int caches_clear_cache(const char *app_name);
int caches_clear_all_caches(void);
int caches_get_cache_size(const char *app_name, uint32_t *size);
int caches_set_cache_limit(uint32_t limit);

#endif
