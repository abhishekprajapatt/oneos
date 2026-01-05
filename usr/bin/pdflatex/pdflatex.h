#ifndef PDFLATEX_H
#define PDFLATEX_H

#include <stdint.h>

typedef struct
{
    const char *source_file;
    const char *output_file;
    uint32_t interaction_mode;
    uint32_t enable_synctex;
    uint32_t shell_escape;
} pdflatex_compile_options_t;

typedef struct
{
    uint32_t total_compilations;
    uint32_t successful;
    uint32_t failed;
    uint64_t total_pages;
} pdflatex_stats_t;

int pdflatex_init(void);
int pdflatex_compile(const char *source_file);
int pdflatex_compile_with_options(pdflatex_compile_options_t *options);
int pdflatex_process_bib(const char *aux_file);
int pdflatex_get_errors(char *error_buffer, uint32_t buffer_size);
int pdflatex_get_stats(pdflatex_stats_t *stats);

#endif
