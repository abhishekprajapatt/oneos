#include "texlive.h"

int texlive_init(void)
{
    return 0;
}

int texlive_install_package(const char *package_name)
{
    if (!package_name)
    {
        return -1;
    }
    return 0;
}

int texlive_install_with_options(texlive_package_options_t *options)
{
    if (!options || !options->package_name)
    {
        return -1;
    }
    return 0;
}

int texlive_remove_package(const char *package_name)
{
    if (!package_name)
    {
        return -1;
    }
    return 0;
}

int texlive_update_all_packages(void)
{
    return 0;
}

int texlive_get_stats(texlive_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
