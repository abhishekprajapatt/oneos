#ifndef WGET_H
#define WGET_H

#include <stdint.h>

typedef struct
{
    const char *url;
    const char *output_file;
    uint32_t continue_download;
    uint32_t max_retry;
    uint32_t timeout;
} wget_download_options_t;

typedef struct
{
    uint32_t total_downloads;
    uint32_t successful;
    uint32_t failed;
    uint64_t total_bytes_downloaded;
} wget_stats_t;

int wget_init(void);
int wget_download_file(const char *url, const char *output_file);
int wget_download_with_options(wget_download_options_t *options);
int wget_download_recursive(const char *url, const char *output_dir);
int wget_mirror_website(const char *url, const char *output_dir);
int wget_get_stats(wget_stats_t *stats);

#endif
