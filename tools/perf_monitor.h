#ifndef PERF_MONITOR_H
#define PERF_MONITOR_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_COUNTERS 256
#define MAX_TRACES 4096
#define MAX_METRICS 512
#define MAX_PROFILING_DATA 2048

typedef enum
{
    COUNTER_CPU_CYCLES = 0,
    COUNTER_INSTRUCTIONS = 1,
    COUNTER_CACHE_HITS = 2,
    COUNTER_CACHE_MISSES = 3,
    COUNTER_BRANCH_MISPREDICTS = 4,
    COUNTER_PAGE_FAULTS = 5,
    COUNTER_CONTEXT_SWITCHES = 6,
    COUNTER_INTERRUPTS = 7
} CounterType;

typedef enum
{
    METRIC_CPU_USAGE = 0,
    METRIC_MEMORY_USAGE = 1,
    METRIC_DISK_IO = 2,
    METRIC_NETWORK_IO = 3,
    METRIC_PROCESS_TIME = 4,
    METRIC_LATENCY = 5,
    METRIC_THROUGHPUT = 6
} MetricType;

typedef enum
{
    TRACE_LEVEL_NONE = 0,
    TRACE_LEVEL_ERROR = 1,
    TRACE_LEVEL_WARNING = 2,
    TRACE_LEVEL_INFO = 3,
    TRACE_LEVEL_DEBUG = 4,
    TRACE_LEVEL_VERBOSE = 5
} TraceLevel;

typedef struct
{
    uint32_t counter_id;
    CounterType type;
    uint64_t value;
    uint64_t delta;
    time_t last_reset;
    uint32_t sample_count;
} Counter;

typedef struct
{
    uint32_t metric_id;
    MetricType type;
    double current_value;
    double min_value;
    double max_value;
    double average_value;
    uint64_t samples_collected;
    time_t last_updated;
} Metric;

typedef struct
{
    uint32_t trace_id;
    TraceLevel level;
    time_t timestamp;
    uint32_t process_id;
    uint32_t thread_id;
    char function_name[128];
    char message[512];
    uint32_t duration_us;
} TraceRecord;

typedef struct
{
    uint32_t profile_id;
    uint32_t process_id;
    uint64_t samples;
    uint64_t cpu_cycles;
    uint64_t instructions;
    float ipc;
    float cache_hit_ratio;
    time_t start_time;
    time_t end_time;
} ProfileData;

typedef struct
{
    Counter counters[MAX_COUNTERS];
    uint32_t counter_count;
    Metric metrics[MAX_METRICS];
    uint32_t metric_count;
    TraceRecord traces[MAX_TRACES];
    uint32_t trace_count;
    ProfileData profiles[MAX_PROFILING_DATA];
    uint32_t profile_count;
    TraceLevel current_trace_level;
    bool profiling_enabled;
    bool tracing_enabled;
    uint64_t total_samples;
    double avg_cpu_usage;
    double avg_memory_usage;
} PerfMonitor;

PerfMonitor *perf_init(void);
void perf_cleanup(PerfMonitor *monitor);

uint32_t perf_create_counter(PerfMonitor *monitor, CounterType type);
int perf_increment_counter(PerfMonitor *monitor, uint32_t counter_id, uint64_t value);
int perf_reset_counter(PerfMonitor *monitor, uint32_t counter_id);
Counter *perf_get_counter(PerfMonitor *monitor, uint32_t counter_id);

uint32_t perf_create_metric(PerfMonitor *monitor, MetricType type);
int perf_record_metric(PerfMonitor *monitor, uint32_t metric_id, double value);
Metric *perf_get_metric(PerfMonitor *monitor, uint32_t metric_id);

int perf_set_trace_level(PerfMonitor *monitor, TraceLevel level);
TraceLevel perf_get_trace_level(PerfMonitor *monitor);
int perf_record_trace(PerfMonitor *monitor, TraceLevel level, uint32_t process_id,
                      uint32_t thread_id, const char *function, const char *message, uint32_t duration_us);
TraceRecord *perf_get_trace(PerfMonitor *monitor, uint32_t trace_id);

int perf_enable_profiling(PerfMonitor *monitor, bool enable);
uint32_t perf_start_profile(PerfMonitor *monitor, uint32_t process_id);
int perf_end_profile(PerfMonitor *monitor, uint32_t profile_id);
ProfileData *perf_get_profile(PerfMonitor *monitor, uint32_t profile_id);

uint32_t perf_get_total_counter_count(PerfMonitor *monitor);
uint32_t perf_get_total_metric_count(PerfMonitor *monitor);
uint32_t perf_get_total_trace_count(PerfMonitor *monitor);

double perf_get_average_cpu_usage(PerfMonitor *monitor);
double perf_get_average_memory_usage(PerfMonitor *monitor);

int perf_generate_report(PerfMonitor *monitor, const char *filename);
int perf_collect_all_metrics(PerfMonitor *monitor);

#endif
