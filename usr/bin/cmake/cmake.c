#include "cmake.h"

int cmake_init(void)
{
    return 0;
}

int cmake_configure(const char *source_dir, const char *build_dir)
{
    if (!source_dir || !build_dir)
    {
        return -1;
    }
    return 0;
}

int cmake_configure_with_options(cmake_config_t *config)
{
    if (!config || !config->source_dir || !config->build_dir)
    {
        return -1;
    }
    return 0;
}

int cmake_build(const char *build_dir)
{
    if (!build_dir)
    {
        return -1;
    }
    return 0;
}

int cmake_install(const char *build_dir, const char *install_dir)
{
    if (!build_dir || !install_dir)
    {
        return -1;
    }
    return 0;
}

int cmake_get_build_stats(cmake_build_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
