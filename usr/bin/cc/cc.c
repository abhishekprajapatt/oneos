#include "cc.h"

int cc_init(void)
{
    return 0;
}

int cc_compile(const char *source, const char *output)
{
    if (!source || !output)
    {
        return -1;
    }
    return 0;
}

int cc_compile_with_options(cc_compile_options_t *options)
{
    if (!options || !options->source_file || !options->output_file)
    {
        return -1;
    }
    return 0;
}

int cc_preprocess(const char *source_file, const char *output_file)
{
    if (!source_file || !output_file)
    {
        return -1;
    }
    return 0;
}

int cc_get_version(char *version, uint32_t max_len)
{
    if (!version || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int cc_get_stats(cc_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
