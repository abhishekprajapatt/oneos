#ifndef CC_H
#define CC_H

#include <stdint.h>

typedef struct
{
    const char *source_file;
    const char *output_file;
    const char *optimization_level;
    uint32_t warning_level;
    uint32_t debug_symbols;
} cc_compile_options_t;

typedef struct
{
    uint32_t total_compilations;
    uint32_t successful;
    uint32_t failed;
    uint64_t total_time;
} cc_stats_t;

int cc_init(void);
int cc_compile(const char *source, const char *output);
int cc_compile_with_options(cc_compile_options_t *options);
int cc_preprocess(const char *source_file, const char *output_file);
int cc_get_version(char *version, uint32_t max_len);
int cc_get_stats(cc_stats_t *stats);

#endif
