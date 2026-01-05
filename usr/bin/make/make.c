#include "make.h"

int make_init(void)
{
    return 0;
}

int make_build(const char *makefile_path)
{
    if (!makefile_path)
    {
        return -1;
    }
    return 0;
}

int make_build_target(const char *makefile_path, const char *target)
{
    if (!makefile_path || !target)
    {
        return -1;
    }
    return 0;
}

int make_build_with_options(make_build_options_t *options)
{
    if (!options || !options->makefile_path)
    {
        return -1;
    }
    return 0;
}

int make_get_targets(const char *makefile_path, char *targets, uint32_t targets_size)
{
    if (!makefile_path || !targets || targets_size == 0)
    {
        return -1;
    }
    return 0;
}

int make_get_build_stats(make_build_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
