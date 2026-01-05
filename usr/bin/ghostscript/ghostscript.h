#ifndef GHOSTSCRIPT_H
#define GHOSTSCRIPT_H

#include <stdint.h>

typedef struct
{
    const char *input_file;
    const char *output_file;
    const char *device;
    uint32_t resolution;
    const char *color_mode;
} ghostscript_convert_options_t;

typedef struct
{
    uint32_t total_conversions;
    uint32_t successful;
    uint32_t failed;
    uint64_t total_pages;
} ghostscript_stats_t;

int ghostscript_init(void);
int ghostscript_convert_ps_to_pdf(const char *ps_file, const char *pdf_file);
int ghostscript_convert_with_options(ghostscript_convert_options_t *options);
int ghostscript_extract_text(const char *ps_file, char *text_buffer, uint32_t buffer_size);
int ghostscript_get_page_count(const char *file, uint32_t *page_count);
int ghostscript_get_stats(ghostscript_stats_t *stats);

#endif
