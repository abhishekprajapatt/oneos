#include "g++.h"

int gpp_init(void)
{
    return 0;
}

int gpp_compile(const char *source, const char *output)
{
    if (!source || !output)
    {
        return -1;
    }
    return 0;
}

int gpp_compile_with_options(gpp_compile_options_t *options)
{
    if (!options || !options->source_file || !options->output_file)
    {
        return -1;
    }
    return 0;
}

int gpp_link_objects(const char **object_files, uint32_t file_count, const char *output)
{
    if (!object_files || file_count == 0 || !output)
    {
        return -1;
    }
    return 0;
}

int gpp_get_version(char *version, uint32_t max_len)
{
    if (!version || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int gpp_get_stats(gpp_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
