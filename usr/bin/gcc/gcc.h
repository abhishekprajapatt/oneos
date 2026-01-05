#ifndef GCC_H
#define GCC_H

#include <stdint.h>

typedef struct
{
    const char *source_file;
    const char *output_file;
    const char *optimization_level;
    uint32_t generate_debug;
    uint32_t wall_warnings;
} gcc_compile_options_t;

typedef struct
{
    uint32_t total_compilations;
    uint32_t successful;
    uint32_t failed;
    uint64_t total_time;
} gcc_stats_t;

int gcc_init(void);
int gcc_compile(const char *source, const char *output);
int gcc_compile_with_options(gcc_compile_options_t *options);
int gcc_assemble(const char *asm_file, const char *output_file);
int gcc_get_version(char *version, uint32_t max_len);
int gcc_get_stats(gcc_stats_t *stats);

#endif
