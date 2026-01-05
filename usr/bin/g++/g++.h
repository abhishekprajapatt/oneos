#ifndef GPP_H
#define GPP_H

#include <stdint.h>

typedef struct
{
    const char *source_file;
    const char *output_file;
    const char *std;
    uint32_t optimization_level;
    uint32_t enable_warnings;
} gpp_compile_options_t;

typedef struct
{
    uint32_t total_compilations;
    uint32_t successful;
    uint32_t failed;
    uint64_t total_compile_time;
} gpp_stats_t;

int gpp_init(void);
int gpp_compile(const char *source, const char *output);
int gpp_compile_with_options(gpp_compile_options_t *options);
int gpp_link_objects(const char **object_files, uint32_t file_count, const char *output);
int gpp_get_version(char *version, uint32_t max_len);
int gpp_get_stats(gpp_stats_t *stats);

#endif
