#ifndef PACKAGE_CACHE_H
#define PACKAGE_CACHE_H

#include <stdint.h>

int package_cache_init(void);
int package_cache_add_repo(const char *repo_url);
int package_cache_remove_repo(const char *repo_url);
int package_cache_list_repos(char **repos, uint32_t max_repos);
int package_cache_sync(void);
int package_cache_search(const char *query, char **results, uint32_t max_results);
int package_cache_clear(void);

#endif
