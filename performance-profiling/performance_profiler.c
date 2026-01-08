#include "performance_profiler.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static PerformanceProfiler *global_profiler = NULL;

PerformanceProfiler *profiler_init(void)
{
    PerformanceProfiler *profiler = (PerformanceProfiler *)malloc(sizeof(PerformanceProfiler));
    if (!profiler)
        return NULL;

    memset(profiler, 0, sizeof(PerformanceProfiler));
    profiler->profiler_id = 1;
    profiler->active_session_count = 0;
    profiler->total_samples_collected = 0;
    profiler->total_samples_dropped = 0;
    profiler->collection_efficiency = 0.0f;

    global_profiler = profiler;
    return profiler;
}

int profiler_destroy(PerformanceProfiler *profiler)
{
    if (!profiler)
        return -1;

    for (uint32_t i = 0; i < MAX_PROFILING_SESSIONS; i++)
    {
        if (profiler->sessions[i].hot_functions)
        {
            free(profiler->sessions[i].hot_functions);
        }
    }

    free(profiler);
    global_profiler = NULL;
    return 0;
}

uint32_t profiling_session_start(PerformanceProfiler *profiler, ProfileType type, uint32_t sample_rate_hz)
{
    if (!profiler || profiler->active_session_count >= MAX_PROFILING_SESSIONS)
        return 0;

    ProfilingSession *session = NULL;
    uint32_t session_id = 0;

    for (uint32_t i = 0; i < MAX_PROFILING_SESSIONS; i++)
    {
        if (profiler->sessions[i].session_id == 0)
        {
            session = &profiler->sessions[i];
            session_id = i + 1;
            break;
        }
    }

    if (!session)
        return 0;

    memset(session, 0, sizeof(ProfilingSession));
    session->session_id = session_id;
    session->profile_type = type;
    session->state = PROFILE_STATE_RECORDING;
    session->sample_count = 0;
    session->hot_function_count = 0;
    session->session_start_time = 0;
    session->session_end_time = 0;
    session->total_profiling_time_us = 0;
    session->sample_rate_hz = (sample_rate_hz > 0) ? sample_rate_hz : 100;
    session->samples_dropped = 0;

    session->hot_functions = (FunctionProfile *)malloc(sizeof(FunctionProfile) * 1024);
    if (!session->hot_functions)
    {
        return 0;
    }

    profiler->active_session_count++;
    return session_id;
}

int profiling_session_stop(PerformanceProfiler *profiler, uint32_t session_id)
{
    if (!profiler || session_id == 0 || session_id > MAX_PROFILING_SESSIONS)
        return -1;

    ProfilingSession *session = &profiler->sessions[session_id - 1];
    if (session->session_id == 0)
        return -1;

    session->state = PROFILE_STATE_STOPPED;
    session->session_end_time = 0;
    session->total_profiling_time_us = (session->session_end_time > session->session_start_time) ? (session->session_end_time - session->session_start_time) : 1000000;

    if (profiler->active_session_count > 0)
        profiler->active_session_count--;

    return 0;
}

int profiling_session_pause(PerformanceProfiler *profiler, uint32_t session_id)
{
    if (!profiler || session_id == 0 || session_id > MAX_PROFILING_SESSIONS)
        return -1;

    ProfilingSession *session = &profiler->sessions[session_id - 1];
    if (session->session_id == 0)
        return -1;

    session->state = PROFILE_STATE_PAUSED;
    return 0;
}

int profiling_session_resume(PerformanceProfiler *profiler, uint32_t session_id)
{
    if (!profiler || session_id == 0 || session_id > MAX_PROFILING_SESSIONS)
        return -1;

    ProfilingSession *session = &profiler->sessions[session_id - 1];
    if (session->session_id == 0)
        return -1;

    session->state = PROFILE_STATE_RECORDING;
    return 0;
}

int profiler_record_sample(PerformanceProfiler *profiler, uint32_t session_id,
                           uint32_t process_id, uint32_t thread_id, uint64_t sample_address)
{
    if (!profiler || session_id == 0 || session_id > MAX_PROFILING_SESSIONS)
        return -1;

    ProfilingSession *session = &profiler->sessions[session_id - 1];
    if (session->session_id == 0 || session->state != PROFILE_STATE_RECORDING)
        return -1;

    if (session->sample_count >= MAX_PROFILE_SAMPLES)
    {
        session->samples_dropped++;
        profiler->total_samples_dropped++;
        return -1;
    }

    ProfileSample *sample = &session->samples[session->sample_count];
    sample->sample_id = session->sample_count;
    sample->timestamp = 0;
    sample->process_id = process_id;
    sample->thread_id = thread_id;
    sample->sample_address = sample_address;
    sample->sample_count = 1;
    sample->call_stack_depth = 0;

    session->sample_count++;
    profiler->total_samples_collected++;

    return 0;
}

int profiler_analyze_session(PerformanceProfiler *profiler, uint32_t session_id)
{
    if (!profiler || session_id == 0 || session_id > MAX_PROFILING_SESSIONS)
        return -1;

    ProfilingSession *session = &profiler->sessions[session_id - 1];
    if (session->session_id == 0)
        return -1;

    session->hot_function_count = 0;

    for (uint32_t i = 0; i < session->sample_count && session->hot_function_count < 1024; i++)
    {
        ProfileSample *sample = &session->samples[i];

        FunctionProfile *func = NULL;
        for (uint32_t j = 0; j < session->hot_function_count; j++)
        {
            if (session->hot_functions[j].function_address == sample->sample_address)
            {
                func = &session->hot_functions[j];
                break;
            }
        }

        if (!func)
        {
            func = &session->hot_functions[session->hot_function_count];
            func->function_address = sample->sample_address;
            snprintf(func->function_name, sizeof(func->function_name), "func_0x%lx", sample->sample_address);
            snprintf(func->module_name, sizeof(func->module_name), "kernel");
            func->total_samples = 0;
            func->total_time_us = 0;
            func->call_count = 0;
            session->hot_function_count++;
        }

        func->total_samples++;
        func->call_count++;
    }

    for (uint32_t i = 0; i < session->hot_function_count; i++)
    {
        if (session->sample_count > 0)
        {
            session->hot_functions[i].percentage_of_samples =
                (float)session->hot_functions[i].total_samples / session->sample_count * 100.0f;
        }
        session->hot_functions[i].hot_score = session->hot_functions[i].total_samples;
    }

    return session->hot_function_count;
}

int profiler_build_call_graph(PerformanceProfiler *profiler, uint32_t session_id)
{
    if (!profiler || session_id == 0 || session_id > MAX_PROFILING_SESSIONS)
        return -1;

    ProfilingSession *session = &profiler->sessions[session_id - 1];
    if (session->session_id == 0)
        return -1;

    for (uint32_t i = 0; i < session->sample_count; i++)
    {
        ProfileSample *sample = &session->samples[i];
        sample->call_stack = (uint64_t *)malloc(sizeof(uint64_t) * 64);
        if (!sample->call_stack)
            return -1;

        sample->call_stack[0] = sample->sample_address;
        sample->call_stack_depth = 1;
    }

    return 0;
}

int profiler_get_hot_functions(PerformanceProfiler *profiler, uint32_t session_id,
                               FunctionProfile *functions, uint32_t max_functions)
{
    if (!profiler || session_id == 0 || session_id > MAX_PROFILING_SESSIONS || !functions)
        return -1;

    ProfilingSession *session = &profiler->sessions[session_id - 1];
    if (session->session_id == 0)
        return -1;

    uint32_t to_copy = session->hot_function_count < max_functions ? session->hot_function_count : max_functions;
    memcpy(functions, session->hot_functions, sizeof(FunctionProfile) * to_copy);

    return to_copy;
}

int profiler_export_results(PerformanceProfiler *profiler, uint32_t session_id, const char *filename)
{
    if (!profiler || session_id == 0 || session_id > MAX_PROFILING_SESSIONS || !filename)
        return -1;

    ProfilingSession *session = &profiler->sessions[session_id - 1];
    if (session->session_id == 0)
        return -1;

    return 0;
}

int profiler_get_metrics(PerformanceProfiler *profiler, ProfilerMetrics *metrics)
{
    if (!profiler || !metrics)
        return -1;

    metrics->profiler_id = profiler->profiler_id;
    metrics->active_sessions = profiler->active_session_count;
    metrics->total_samples = profiler->total_samples_collected;
    metrics->samples_dropped = profiler->total_samples_dropped;

    if (profiler->total_samples_collected + profiler->total_samples_dropped > 0)
    {
        metrics->collection_efficiency = (float)profiler->total_samples_collected /
                                         (profiler->total_samples_collected + profiler->total_samples_dropped);
    }
    else
    {
        metrics->collection_efficiency = 0.0f;
    }

    return 0;
}
