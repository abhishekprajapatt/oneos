#ifndef APT_CACHE_H
#define APT_CACHE_H

#include <stdint.h>

int apt_cache_init(void);
int apt_cache_update(void);
int apt_cache_search(const char *query);
int apt_cache_show_package(const char *package_name);
int apt_cache_depends(const char *package_name);
int apt_cache_clean(void);
int apt_cache_autoclean(void);

#endif
