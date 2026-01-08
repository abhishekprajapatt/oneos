#ifndef PERFORMANCE_PROFILER_H
#define PERFORMANCE_PROFILER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_PROFILING_SESSIONS 128
#define MAX_PROFILE_SAMPLES 65536
#define MAX_CALL_STACKS 16384

typedef enum
{
    PROFILE_CPU,
    PROFILE_MEMORY,
    PROFILE_IO,
    PROFILE_CONTEXT_SWITCH,
    PROFILE_CACHE_MISS
} ProfileType;

typedef enum
{
    PROFILE_STATE_IDLE,
    PROFILE_STATE_RECORDING,
    PROFILE_STATE_PAUSED,
    PROFILE_STATE_STOPPED
} ProfileState;

typedef struct
{
    uint32_t sample_id;
    uint64_t timestamp;
    uint32_t process_id;
    uint32_t thread_id;

    uint64_t sample_address;
    uint32_t sample_count;

    uint64_t *call_stack;
    uint32_t call_stack_depth;
} ProfileSample;

typedef struct
{
    uint32_t function_address;
    char function_name[256];
    char module_name[256];

    uint64_t total_samples;
    uint64_t total_time_us;
    uint32_t call_count;

    float percentage_of_samples;
    uint64_t hot_score;
} FunctionProfile;

typedef struct
{
    uint32_t session_id;
    ProfileType profile_type;
    ProfileState state;

    ProfileSample samples[MAX_PROFILE_SAMPLES];
    uint32_t sample_count;

    FunctionProfile *hot_functions;
    uint32_t hot_function_count;

    uint64_t session_start_time;
    uint64_t session_end_time;
    uint64_t total_profiling_time_us;

    uint32_t sample_rate_hz;
    uint32_t samples_dropped;
} ProfilingSession;

typedef struct
{
    uint32_t profiler_id;
    ProfilingSession sessions[MAX_PROFILING_SESSIONS];
    uint32_t active_session_count;

    uint64_t total_samples_collected;
    uint64_t total_samples_dropped;
    float collection_efficiency;
} PerformanceProfiler;

PerformanceProfiler *profiler_init(void);
int profiler_destroy(PerformanceProfiler *profiler);

uint32_t profiling_session_start(PerformanceProfiler *profiler, ProfileType type, uint32_t sample_rate_hz);
int profiling_session_stop(PerformanceProfiler *profiler, uint32_t session_id);
int profiling_session_pause(PerformanceProfiler *profiler, uint32_t session_id);
int profiling_session_resume(PerformanceProfiler *profiler, uint32_t session_id);

int profiler_record_sample(PerformanceProfiler *profiler, uint32_t session_id,
                           uint32_t process_id, uint32_t thread_id, uint64_t sample_address);

int profiler_analyze_session(PerformanceProfiler *profiler, uint32_t session_id);
int profiler_build_call_graph(PerformanceProfiler *profiler, uint32_t session_id);

int profiler_get_hot_functions(PerformanceProfiler *profiler, uint32_t session_id,
                               FunctionProfile *functions, uint32_t max_functions);

int profiler_export_results(PerformanceProfiler *profiler, uint32_t session_id, const char *filename);

typedef struct
{
    uint32_t profiler_id;
    uint32_t active_sessions;
    uint64_t total_samples;
    uint64_t samples_dropped;
    float collection_efficiency;
} ProfilerMetrics;

int profiler_get_metrics(PerformanceProfiler *profiler, ProfilerMetrics *metrics);

#endif
