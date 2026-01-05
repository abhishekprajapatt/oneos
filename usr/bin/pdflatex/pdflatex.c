#include "pdflatex.h"

int pdflatex_init(void)
{
    return 0;
}

int pdflatex_compile(const char *source_file)
{
    if (!source_file)
    {
        return -1;
    }
    return 0;
}

int pdflatex_compile_with_options(pdflatex_compile_options_t *options)
{
    if (!options || !options->source_file)
    {
        return -1;
    }
    return 0;
}

int pdflatex_process_bib(const char *aux_file)
{
    if (!aux_file)
    {
        return -1;
    }
    return 0;
}

int pdflatex_get_errors(char *error_buffer, uint32_t buffer_size)
{
    if (!error_buffer || buffer_size == 0)
    {
        return -1;
    }
    return 0;
}

int pdflatex_get_stats(pdflatex_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
