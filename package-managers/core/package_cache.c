#include "package_cache.h"

int package_cache_init(void)
{
    return 0;
}

int package_cache_add_repo(const char *repo_url)
{
    if (!repo_url)
    {
        return -1;
    }
    return 0;
}

int package_cache_remove_repo(const char *repo_url)
{
    if (!repo_url)
    {
        return -1;
    }
    return 0;
}

int package_cache_list_repos(char **repos, uint32_t max_repos)
{
    if (!repos || max_repos == 0)
    {
        return -1;
    }
    return 0;
}

int package_cache_sync(void)
{
    return 0;
}

int package_cache_search(const char *query, char **results, uint32_t max_results)
{
    if (!query || !results || max_results == 0)
    {
        return -1;
    }
    return 0;
}

int package_cache_clear(void)
{
    return 0;
}
