#include "dvips.h"

int dvips_init(void)
{
    return 0;
}

int dvips_convert(const char *dvi_file, const char *ps_file)
{
    if (!dvi_file || !ps_file)
    {
        return -1;
    }
    return 0;
}

int dvips_convert_with_options(dvips_options_t *options)
{
    if (!options || !options->dvi_file || !options->ps_file)
    {
        return -1;
    }
    return 0;
}

int dvips_get_dvi_info(const char *dvi_file, char *info, uint32_t info_size)
{
    if (!dvi_file || !info || info_size == 0)
    {
        return -1;
    }
    return 0;
}

int dvips_get_stats(dvips_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}

int dvips_cleanup(void)
{
    return 0;
}
