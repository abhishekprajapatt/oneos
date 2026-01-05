#ifndef DVIPS_H
#define DVIPS_H

#include <stdint.h>

typedef struct
{
    const char *dvi_file;
    const char *ps_file;
    uint32_t resolution;
    const char *paper_size;
    uint32_t compressed;
} dvips_options_t;

typedef struct
{
    uint32_t total_conversions;
    uint32_t successful;
    uint32_t failed;
    uint64_t total_pages;
} dvips_stats_t;

int dvips_init(void);
int dvips_convert(const char *dvi_file, const char *ps_file);
int dvips_convert_with_options(dvips_options_t *options);
int dvips_get_dvi_info(const char *dvi_file, char *info, uint32_t info_size);
int dvips_get_stats(dvips_stats_t *stats);
int dvips_cleanup(void);

#endif
