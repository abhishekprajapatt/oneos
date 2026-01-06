#include "benchmark.h"

int benchmark_init(void)
{
    return 0;
}

int benchmark_run_cpu_test(void)
{
    return 0;
}

int benchmark_run_memory_test(void)
{
    return 0;
}

int benchmark_run_io_test(void)
{
    return 0;
}

int benchmark_get_results(benchmark_result_t **results, uint32_t *count)
{
    if (!results || !count)
    {
        return -1;
    }
    *count = 0;
    return 0;
}

int benchmark_generate_report(const char *output_file)
{
    if (!output_file)
    {
        return -1;
    }
    return 0;
}
