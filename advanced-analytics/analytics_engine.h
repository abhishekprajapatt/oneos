#ifndef ANALYTICS_ENGINE_H
#define ANALYTICS_ENGINE_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_ANALYTICS_ENGINES 64
#define MAX_DATA_STREAMS 512
#define MAX_ANALYTICS_JOBS 256
#define MAX_METRICS 1024
#define MAX_EVENTS 2048
#define STREAM_BUFFER_SIZE 65536

typedef enum
{
    STREAM_TYPE_METRICS,
    STREAM_TYPE_LOGS,
    STREAM_TYPE_EVENTS,
    STREAM_TYPE_TRACES,
    STREAM_TYPE_CUSTOM
} StreamType;

typedef enum
{
    PROCESSING_MODE_REAL_TIME,
    PROCESSING_MODE_BATCH,
    PROCESSING_MODE_STREAM,
    PROCESSING_MODE_HYBRID
} ProcessingMode;

typedef enum
{
    AGGREGATION_SUM,
    AGGREGATION_AVG,
    AGGREGATION_MIN,
    AGGREGATION_MAX,
    AGGREGATION_COUNT,
    AGGREGATION_STDDEV,
    AGGREGATION_PERCENTILE,
    AGGREGATION_HISTOGRAM
} AggregationType;

typedef struct
{
    uint32_t stream_id;
    StreamType type;
    char stream_name[128];
    ProcessingMode mode;
    uint64_t event_count;
    uint64_t bytes_received;
    uint64_t bytes_processed;
    float throughput_mbps;
    bool active;
    time_t created_at;
    time_t last_event_at;
    uint8_t *buffer;
    uint32_t buffer_pos;
} DataStream;

typedef struct
{
    uint32_t metric_id;
    char metric_name[128];
    float value;
    time_t timestamp;
    float min_value;
    float max_value;
    float avg_value;
    uint64_t sample_count;
    AggregationType aggregation;
    bool is_derived;
} Metric;

typedef struct
{
    uint32_t job_id;
    uint32_t stream_id;
    char job_name[128];
    ProcessingMode mode;
    uint32_t batch_size;
    uint64_t processed_records;
    uint64_t failed_records;
    float success_rate;
    time_t start_time;
    time_t end_time;
    bool is_running;
    bool completed;
} AnalyticsJob;

typedef struct
{
    uint32_t engine_id;
    char engine_name[128];
    ProcessingMode mode;
    uint32_t stream_count;
    uint32_t job_count;
    uint32_t metric_count;
    uint64_t total_events_processed;
    time_t created_at;
    bool active;
} AnalyticsEngine;

typedef struct
{
    AnalyticsEngine engines[MAX_ANALYTICS_ENGINES];
    DataStream streams[MAX_DATA_STREAMS];
    AnalyticsJob jobs[MAX_ANALYTICS_JOBS];
    Metric metrics[MAX_METRICS];
    uint32_t engine_count;
    uint32_t stream_count;
    uint32_t job_count;
    uint32_t metric_count;
} AnalyticsInfrastructure;

AnalyticsInfrastructure *analytics_init(void);
void analytics_cleanup(AnalyticsInfrastructure *analytics);

uint32_t analytics_create_engine(AnalyticsInfrastructure *analytics, const char *name, ProcessingMode mode);
int analytics_delete_engine(AnalyticsInfrastructure *analytics, uint32_t engine_id);

uint32_t analytics_create_stream(AnalyticsInfrastructure *analytics, uint32_t engine_id, const char *name, StreamType type, ProcessingMode mode);
int analytics_push_event(AnalyticsInfrastructure *analytics, uint32_t stream_id, const uint8_t *data, uint32_t size);
int analytics_get_stream_throughput(AnalyticsInfrastructure *analytics, uint32_t stream_id, float *throughput);

uint32_t analytics_create_metric(AnalyticsInfrastructure *analytics, const char *name, AggregationType agg);
int analytics_update_metric(AnalyticsInfrastructure *analytics, uint32_t metric_id, float value);
int analytics_get_metric_stats(AnalyticsInfrastructure *analytics, uint32_t metric_id, float *min, float *max, float *avg);

uint32_t analytics_create_job(AnalyticsInfrastructure *analytics, uint32_t stream_id, const char *name, ProcessingMode mode);
int analytics_start_job(AnalyticsInfrastructure *analytics, uint32_t job_id);
int analytics_stop_job(AnalyticsInfrastructure *analytics, uint32_t job_id);
int analytics_get_job_progress(AnalyticsInfrastructure *analytics, uint32_t job_id, uint64_t *processed, uint64_t *failed);

int analytics_aggregate_metrics(AnalyticsInfrastructure *analytics, uint32_t engine_id, time_t start_time, time_t end_time);
int analytics_query_metrics(AnalyticsInfrastructure *analytics, const char *query, Metric **results, uint32_t *count);

#endif
