#include "gcc.h"

int gcc_init(void)
{
    return 0;
}

int gcc_compile(const char *source, const char *output)
{
    if (!source || !output)
    {
        return -1;
    }
    return 0;
}

int gcc_compile_with_options(gcc_compile_options_t *options)
{
    if (!options || !options->source_file || !options->output_file)
    {
        return -1;
    }
    return 0;
}

int gcc_assemble(const char *asm_file, const char *output_file)
{
    if (!asm_file || !output_file)
    {
        return -1;
    }
    return 0;
}

int gcc_get_version(char *version, uint32_t max_len)
{
    if (!version || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int gcc_get_stats(gcc_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
