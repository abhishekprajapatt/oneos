#include "easy_install.h"

int easy_install_init(void)
{
    return 0;
}

int easy_install_package(const char *package_name)
{
    if (!package_name)
    {
        return -1;
    }
    return 0;
}

int easy_install_with_options(easy_install_options_t *options)
{
    if (!options || !options->package_name)
    {
        return -1;
    }
    return 0;
}

int easy_install_search(const char *package_name, char *results, uint32_t results_size)
{
    if (!package_name || !results || results_size == 0)
    {
        return -1;
    }
    return 0;
}

int easy_install_remove(const char *package_name)
{
    if (!package_name)
    {
        return -1;
    }
    return 0;
}

int easy_install_get_stats(easy_install_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
