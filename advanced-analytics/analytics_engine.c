#include "analytics_engine.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

AnalyticsInfrastructure *analytics_init(void)
{
    AnalyticsInfrastructure *analytics = (AnalyticsInfrastructure *)malloc(sizeof(AnalyticsInfrastructure));
    if (!analytics)
        return NULL;

    memset(analytics, 0, sizeof(AnalyticsInfrastructure));
    analytics->engine_count = 0;
    analytics->stream_count = 0;
    analytics->job_count = 0;
    analytics->metric_count = 0;

    return analytics;
}

void analytics_cleanup(AnalyticsInfrastructure *analytics)
{
    if (!analytics)
        return;

    for (uint32_t i = 0; i < analytics->stream_count; i++)
    {
        if (analytics->streams[i].buffer)
            free(analytics->streams[i].buffer);
    }

    memset(analytics, 0, sizeof(AnalyticsInfrastructure));
    free(analytics);
}

uint32_t analytics_create_engine(AnalyticsInfrastructure *analytics, const char *name, ProcessingMode mode)
{
    if (!analytics || !name || analytics->engine_count >= MAX_ANALYTICS_ENGINES)
        return 0;

    AnalyticsEngine *engine = &analytics->engines[analytics->engine_count];
    engine->engine_id = analytics->engine_count + 1;
    strncpy(engine->engine_name, name, 127);
    engine->mode = mode;
    engine->stream_count = 0;
    engine->job_count = 0;
    engine->metric_count = 0;
    engine->total_events_processed = 0;
    engine->created_at = time(NULL);
    engine->active = true;

    analytics->engine_count++;
    return engine->engine_id;
}

int analytics_delete_engine(AnalyticsInfrastructure *analytics, uint32_t engine_id)
{
    if (!analytics || engine_id == 0 || engine_id > analytics->engine_count)
        return -1;

    AnalyticsEngine *engine = &analytics->engines[engine_id - 1];
    engine->active = false;

    return 0;
}

uint32_t analytics_create_stream(AnalyticsInfrastructure *analytics, uint32_t engine_id, const char *name, StreamType type, ProcessingMode mode)
{
    if (!analytics || engine_id == 0 || engine_id > analytics->engine_count || !name || analytics->stream_count >= MAX_DATA_STREAMS)
        return 0;

    DataStream *stream = &analytics->streams[analytics->stream_count];
    stream->stream_id = analytics->stream_count + 1;
    strncpy(stream->stream_name, name, 127);
    stream->type = type;
    stream->mode = mode;
    stream->event_count = 0;
    stream->bytes_received = 0;
    stream->bytes_processed = 0;
    stream->throughput_mbps = 0.0f;
    stream->active = true;
    stream->created_at = time(NULL);
    stream->last_event_at = stream->created_at;
    stream->buffer = (uint8_t *)malloc(STREAM_BUFFER_SIZE);
    stream->buffer_pos = 0;

    analytics->streams[analytics->stream_count] = *stream;
    analytics->engine_count++;
    analytics->stream_count++;

    return stream->stream_id;
}

int analytics_push_event(AnalyticsInfrastructure *analytics, uint32_t stream_id, const uint8_t *data, uint32_t size)
{
    if (!analytics || stream_id == 0 || stream_id > analytics->stream_count || !data || size == 0)
        return -1;

    DataStream *stream = &analytics->streams[stream_id - 1];

    if (stream->buffer_pos + size > STREAM_BUFFER_SIZE)
        stream->buffer_pos = 0;

    memcpy(stream->buffer + stream->buffer_pos, data, size);
    stream->buffer_pos += size;
    stream->event_count++;
    stream->bytes_received += size;
    stream->last_event_at = time(NULL);

    return 0;
}

int analytics_get_stream_throughput(AnalyticsInfrastructure *analytics, uint32_t stream_id, float *throughput)
{
    if (!analytics || stream_id == 0 || stream_id > analytics->stream_count || !throughput)
        return -1;

    DataStream *stream = &analytics->streams[stream_id - 1];
    time_t now = time(NULL);
    time_t elapsed = (now - stream->created_at > 0) ? (now - stream->created_at) : 1;

    *throughput = (float)stream->bytes_processed / (1024.0f * 1024.0f * (float)elapsed);

    return 0;
}

uint32_t analytics_create_metric(AnalyticsInfrastructure *analytics, const char *name, AggregationType agg)
{
    if (!analytics || !name || analytics->metric_count >= MAX_METRICS)
        return 0;

    Metric *metric = &analytics->metrics[analytics->metric_count];
    metric->metric_id = analytics->metric_count + 1;
    strncpy(metric->metric_name, name, 127);
    metric->value = 0.0f;
    metric->timestamp = time(NULL);
    metric->min_value = 0.0f;
    metric->max_value = 0.0f;
    metric->avg_value = 0.0f;
    metric->sample_count = 0;
    metric->aggregation = agg;
    metric->is_derived = false;

    analytics->metric_count++;
    return metric->metric_id;
}

int analytics_update_metric(AnalyticsInfrastructure *analytics, uint32_t metric_id, float value)
{
    if (!analytics || metric_id == 0 || metric_id > analytics->metric_count)
        return -1;

    Metric *metric = &analytics->metrics[metric_id - 1];
    metric->value = value;
    metric->timestamp = time(NULL);

    if (metric->sample_count == 0)
    {
        metric->min_value = value;
        metric->max_value = value;
    }
    else
    {
        if (value < metric->min_value)
            metric->min_value = value;
        if (value > metric->max_value)
            metric->max_value = value;
    }

    metric->avg_value = (metric->avg_value * metric->sample_count + value) / (metric->sample_count + 1);
    metric->sample_count++;

    return 0;
}

int analytics_get_metric_stats(AnalyticsInfrastructure *analytics, uint32_t metric_id, float *min, float *max, float *avg)
{
    if (!analytics || metric_id == 0 || metric_id > analytics->metric_count || !min || !max || !avg)
        return -1;

    Metric *metric = &analytics->metrics[metric_id - 1];
    *min = metric->min_value;
    *max = metric->max_value;
    *avg = metric->avg_value;

    return 0;
}

uint32_t analytics_create_job(AnalyticsInfrastructure *analytics, uint32_t stream_id, const char *name, ProcessingMode mode)
{
    if (!analytics || stream_id == 0 || stream_id > analytics->stream_count || !name || analytics->job_count >= MAX_ANALYTICS_JOBS)
        return 0;

    AnalyticsJob *job = &analytics->jobs[analytics->job_count];
    job->job_id = analytics->job_count + 1;
    job->stream_id = stream_id;
    strncpy(job->job_name, name, 127);
    job->mode = mode;
    job->batch_size = 1000;
    job->processed_records = 0;
    job->failed_records = 0;
    job->success_rate = 0.0f;
    job->start_time = 0;
    job->end_time = 0;
    job->is_running = false;
    job->completed = false;

    analytics->job_count++;
    return job->job_id;
}

int analytics_start_job(AnalyticsInfrastructure *analytics, uint32_t job_id)
{
    if (!analytics || job_id == 0 || job_id > analytics->job_count)
        return -1;

    AnalyticsJob *job = &analytics->jobs[job_id - 1];
    job->is_running = true;
    job->start_time = time(NULL);
    job->processed_records = 0;
    job->failed_records = 0;

    return 0;
}

int analytics_stop_job(AnalyticsInfrastructure *analytics, uint32_t job_id)
{
    if (!analytics || job_id == 0 || job_id > analytics->job_count)
        return -1;

    AnalyticsJob *job = &analytics->jobs[job_id - 1];
    job->is_running = false;
    job->completed = true;
    job->end_time = time(NULL);

    if (job->processed_records > 0)
        job->success_rate = 100.0f * (float)(job->processed_records - job->failed_records) / (float)job->processed_records;

    return 0;
}

int analytics_get_job_progress(AnalyticsInfrastructure *analytics, uint32_t job_id, uint64_t *processed, uint64_t *failed)
{
    if (!analytics || job_id == 0 || job_id > analytics->job_count || !processed || !failed)
        return -1;

    AnalyticsJob *job = &analytics->jobs[job_id - 1];
    *processed = job->processed_records;
    *failed = job->failed_records;

    return 0;
}

int analytics_aggregate_metrics(AnalyticsInfrastructure *analytics, uint32_t engine_id, time_t start_time, time_t end_time)
{
    if (!analytics || engine_id == 0 || engine_id > analytics->engine_count)
        return -1;

    for (uint32_t i = 0; i < analytics->metric_count; i++)
    {
        if (analytics->metrics[i].timestamp >= start_time && analytics->metrics[i].timestamp <= end_time)
        {
            analytics->engines[engine_id - 1].metric_count++;
        }
    }

    return 0;
}

int analytics_query_metrics(AnalyticsInfrastructure *analytics, const char *query, Metric **results, uint32_t *count)
{
    if (!analytics || !query || !results || !count)
        return -1;

    *count = 0;

    for (uint32_t i = 0; i < analytics->metric_count; i++)
    {
        if (strstr(analytics->metrics[i].metric_name, query) != NULL)
        {
            results[*count] = &analytics->metrics[i];
            (*count)++;
            if (*count >= MAX_METRICS)
                break;
        }
    }

    return 0;
}
