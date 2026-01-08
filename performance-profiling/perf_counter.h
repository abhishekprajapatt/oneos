#ifndef PERF_COUNTER_H
#define PERF_COUNTER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_PERF_COUNTERS 256
#define MAX_CPU_CORES 256
#define MAX_COUNTER_EVENTS 32

typedef enum
{
    PERF_EVENT_CPU_CYCLES,
    PERF_EVENT_INSTRUCTIONS,
    PERF_EVENT_CACHE_MISSES,
    PERF_EVENT_CACHE_REFERENCES,
    PERF_EVENT_BRANCH_MISSES,
    PERF_EVENT_PAGE_FAULTS,
    PERF_EVENT_CONTEXT_SWITCHES,
    PERF_EVENT_CPU_MIGRATIONS
} PerfEventType;

typedef struct
{
    PerfEventType event_type;
    char event_name[64];

    uint64_t count;
    uint64_t peak_count;
    uint64_t min_count;
    uint64_t max_count;

    uint32_t measurement_count;
    float average;
} PerfCounter;

typedef struct
{
    uint32_t counter_id;
    uint32_t core_id;

    PerfCounter counters[MAX_COUNTER_EVENTS];
    uint32_t counter_count;

    uint64_t enabled_time_ns;
    uint64_t running_time_ns;

    float multiplexing_ratio;
} CorePerfCounters;

typedef struct
{
    uint32_t manager_id;
    uint32_t core_count;
    CorePerfCounters cores[MAX_CPU_CORES];

    uint64_t total_cycles;
    uint64_t total_instructions;
    uint64_t total_cache_misses;
    uint64_t total_page_faults;

    float ipc;
    float cache_miss_rate;
    float context_switch_rate;
} PerfCounterManager;

PerfCounterManager *perf_counter_init(uint32_t core_count);
int perf_counter_destroy(PerfCounterManager *manager);

int perf_counter_add_event(PerfCounterManager *manager, uint32_t core_id, PerfEventType event);
int perf_counter_remove_event(PerfCounterManager *manager, uint32_t core_id, PerfEventType event);

int perf_counter_enable(PerfCounterManager *manager);
int perf_counter_disable(PerfCounterManager *manager);

int perf_counter_read(PerfCounterManager *manager, uint32_t core_id, PerfEventType event, uint64_t *count);
int perf_counter_reset(PerfCounterManager *manager, uint32_t core_id);
int perf_counter_reset_all(PerfCounterManager *manager);

int perf_counter_calculate_ipc(PerfCounterManager *manager);
int perf_counter_calculate_cache_miss_rate(PerfCounterManager *manager);

typedef struct
{
    uint32_t manager_id;
    uint32_t core_count;
    uint64_t total_cycles;
    uint64_t total_instructions;
    float ipc;
    float cache_miss_rate;
} PerfMetrics;

int perf_counter_get_metrics(PerfCounterManager *manager, PerfMetrics *metrics);

#endif
