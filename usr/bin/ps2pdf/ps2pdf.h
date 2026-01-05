#ifndef PS2PDF_H
#define PS2PDF_H

#include <stdint.h>

typedef struct
{
    const char *ps_file;
    const char *pdf_file;
    const char *device;
    uint32_t resolution;
    uint32_t page_size;
} ps2pdf_convert_options_t;

typedef struct
{
    uint32_t total_conversions;
    uint32_t successful;
    uint32_t failed;
    uint64_t total_pages;
} ps2pdf_stats_t;

int ps2pdf_init(void);
int ps2pdf_convert(const char *ps_file, const char *pdf_file);
int ps2pdf_convert_with_options(ps2pdf_convert_options_t *options);
int ps2pdf_get_ps_info(const char *ps_file, char *info, uint32_t info_size);
int ps2pdf_set_resolution(uint32_t dpi);
int ps2pdf_get_stats(ps2pdf_stats_t *stats);

#endif
