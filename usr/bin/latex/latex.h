#ifndef LATEX_H
#define LATEX_H

#include <stdint.h>

typedef struct
{
    const char *source_file;
    const char *output_file;
    const char *output_format;
    uint32_t interaction_mode;
    uint32_t bib_processing;
} latex_compile_options_t;

typedef struct
{
    uint32_t total_compilations;
    uint32_t successful;
    uint32_t failed;
    uint64_t total_pages;
} latex_stats_t;

int latex_init(void);
int latex_compile(const char *source_file);
int latex_compile_with_options(latex_compile_options_t *options);
int latex_bibtex_process(const char *aux_file);
int latex_get_compile_errors(char *error_buffer, uint32_t buffer_size);
int latex_get_stats(latex_stats_t *stats);

#endif
