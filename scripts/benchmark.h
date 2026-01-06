#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdint.h>

typedef struct
{
    const char *test_name;
    uint64_t execution_time;
    uint32_t memory_usage;
} benchmark_result_t;

int benchmark_init(void);
int benchmark_run_cpu_test(void);
int benchmark_run_memory_test(void);
int benchmark_run_io_test(void);
int benchmark_get_results(benchmark_result_t **results, uint32_t *count);
int benchmark_generate_report(const char *output_file);

#endif
