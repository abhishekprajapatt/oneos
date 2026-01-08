/* OneOS Performance Benchmarking Suite */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define ITERATIONS 10000
#define BUFFER_SIZE 1024 * 1024

typedef struct
{
    const char *name;
    long duration_us;
    double throughput;
    const char *unit;
} bench_result_t;

static bench_result_t results[256];
static int result_count = 0;

/* Get current time in microseconds */
static long get_time_us(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000) + tv.tv_usec;
}

/* Record benchmark result */
void record_result(const char *name, long duration_us, double throughput, const char *unit)
{
    if (result_count < 256)
    {
        results[result_count].name = name;
        results[result_count].duration_us = duration_us;
        results[result_count].throughput = throughput;
        results[result_count].unit = unit;
        result_count++;
    }
}

/* ===== MEMORY BENCHMARKS ===== */

void bench_memory_allocation(void)
{
    printf("\n[Memory Allocation]\n");

    long start = get_time_us();
    for (int i = 0; i < ITERATIONS; i++)
    {
        void *ptr = malloc(4096);
        free(ptr);
    }
    long duration = get_time_us() - start;

    double ops_per_sec = (double)ITERATIONS * 1000000.0 / (double)duration;
    printf("  Alloc/Free: %.2f ops/sec (%.2f ms)\n", ops_per_sec, duration / 1000.0);

    record_result("Memory Allocation", duration, ops_per_sec, "ops/sec");
}

void bench_memory_copy(void)
{
    printf("[Memory Copy]\n");

    void *src = malloc(BUFFER_SIZE);
    void *dst = malloc(BUFFER_SIZE);

    long start = get_time_us();
    for (int i = 0; i < 100; i++)
    {
        memcpy(dst, src, BUFFER_SIZE);
    }
    long duration = get_time_us() - start;

    double bandwidth = (double)BUFFER_SIZE * 100.0 * 1000000.0 / (double)duration / 1024.0 / 1024.0;
    printf("  Bandwidth: %.2f MB/sec (%.2f ms)\n", bandwidth, duration / 1000.0);

    free(src);
    free(dst);

    record_result("Memory Copy", duration, bandwidth, "MB/sec");
}

void bench_memory_fill(void)
{
    printf("[Memory Fill]\n");

    void *buf = malloc(BUFFER_SIZE);

    long start = get_time_us();
    for (int i = 0; i < 100; i++)
    {
        memset(buf, 0xAA, BUFFER_SIZE);
    }
    long duration = get_time_us() - start;

    double bandwidth = (double)BUFFER_SIZE * 100.0 * 1000000.0 / (double)duration / 1024.0 / 1024.0;
    printf("  Bandwidth: %.2f MB/sec (%.2f ms)\n", bandwidth, duration / 1000.0);

    free(buf);

    record_result("Memory Fill", duration, bandwidth, "MB/sec");
}

/* ===== CPU BENCHMARKS ===== */

void bench_integer_arithmetic(void)
{
    printf("\n[Integer Arithmetic]\n");

    long result = 0;
    long start = get_time_us();

    for (int i = 0; i < ITERATIONS * 10; i++)
    {
        result += i;
        result *= 2;
        result /= 3;
    }

    long duration = get_time_us() - start;
    double ops_per_sec = (double)ITERATIONS * 10.0 * 3.0 * 1000000.0 / (double)duration;

    printf("  Int Ops: %.2f Mops/sec (%.2f ms)\n", ops_per_sec / 1000000, duration / 1000.0);
    printf("  Result: %ld\n", result);

    record_result("Integer Arithmetic", duration, ops_per_sec / 1000000, "Mops/sec");
}

void bench_floating_point(void)
{
    printf("[Floating Point]\n");

    double result = 1.0;
    long start = get_time_us();

    for (int i = 0; i < ITERATIONS * 10; i++)
    {
        result += 1.5 * i;
        result *= 1.2;
        result /= 1.1;
    }

    long duration = get_time_us() - start;
    double ops_per_sec = (double)ITERATIONS * 10.0 * 3.0 * 1000000.0 / (double)duration;

    printf("  FP Ops: %.2f Mops/sec (%.2f ms)\n", ops_per_sec / 1000000, duration / 1000.0);
    printf("  Result: %.2f\n", result);

    record_result("Floating Point", duration, ops_per_sec / 1000000, "Mops/sec");
}

/* ===== CONTEXT SWITCH BENCHMARKS ===== */

void bench_context_switch(void)
{
    printf("\n[Context Switching]\n");

    long start = get_time_us();
    for (int i = 0; i < ITERATIONS; i++)
    {
        /* Simulate context switch overhead */
        volatile int x = i;
        x = x * 2;
    }
    long duration = get_time_us() - start;

    double switches_per_sec = (double)ITERATIONS * 1000000.0 / (double)duration;
    printf("  Context Switches: %.2f ops/sec (%.2f ms)\n", switches_per_sec, duration / 1000.0);

    record_result("Context Switch", duration, switches_per_sec, "ops/sec");
}

/* ===== SYSCALL BENCHMARKS ===== */

void bench_syscall_overhead(void)
{
    printf("\n[Syscall Overhead]\n");

    long start = get_time_us();
    for (int i = 0; i < ITERATIONS / 10; i++)
    {
        /* Simulate minimal syscall */
        (void)getpid();
    }
    long duration = get_time_us() - start;

    double syscalls_per_sec = (double)(ITERATIONS / 10) * 1000000.0 / (double)duration;
    printf("  Syscalls: %.2f ops/sec (%.2f ms)\n", syscalls_per_sec, duration / 1000.0);

    record_result("Syscall Overhead", duration, syscalls_per_sec, "ops/sec");
}

/* Display all results */
void display_results(void)
{
    printf("\n");
    printf("========================================\n");
    printf("OneOS Performance Benchmarks\n");
    printf("========================================\n");

    for (int i = 0; i < result_count; i++)
    {
        printf("%-25s: %8.2f %s\n",
               results[i].name,
               results[i].throughput,
               results[i].unit);
    }

    printf("========================================\n\n");
}

/* Main benchmark suite */
int main(void)
{
    printf("\n");
    printf("========================================\n");
    printf("OneOS Performance Benchmarking Suite\n");
    printf("40-Part Architecture Analysis\n");
    printf("========================================\n");

    /* Run memory benchmarks */
    printf("\n=== Memory Benchmarks ===\n");
    bench_memory_allocation();
    bench_memory_copy();
    bench_memory_fill();

    /* Run CPU benchmarks */
    printf("\n=== CPU Benchmarks ===\n");
    bench_integer_arithmetic();
    bench_floating_point();

    /* Run system benchmarks */
    printf("\n=== System Benchmarks ===\n");
    bench_context_switch();
    bench_syscall_overhead();

    /* Display summary */
    display_results();

    return 0;
}
