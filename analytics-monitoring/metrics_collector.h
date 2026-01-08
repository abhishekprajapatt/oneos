#ifndef METRICS_COLLECTOR_H
#define METRICS_COLLECTOR_H

#include <stdint.h>
#include <stddef.h>

#define MAX_METRICS 2048
#define MAX_METRIC_SAMPLES 8192
#define MAX_COLLECTORS 256
#define MAX_AGGREGATIONS 512

typedef enum
{
    METRIC_TYPE_COUNTER,
    METRIC_TYPE_GAUGE,
    METRIC_TYPE_HISTOGRAM,
    METRIC_TYPE_DISTRIBUTION,
    METRIC_TYPE_RATE
} MetricType;

typedef enum
{
    METRIC_SOURCE_CPU,
    METRIC_SOURCE_MEMORY,
    METRIC_SOURCE_DISK,
    METRIC_SOURCE_NETWORK,
    METRIC_SOURCE_PROCESS,
    METRIC_SOURCE_APPLICATION
} MetricSource;

typedef struct
{
    uint64_t timestamp;
    double value;
    uint32_t sample_id;
} MetricSample;

typedef struct
{
    uint32_t metric_id;
    char metric_name[128];
    MetricType metric_type;
    MetricSource source;

    MetricSample samples[MAX_METRIC_SAMPLES];
    uint32_t sample_count;

    double current_value;
    double min_value;
    double max_value;
    double average_value;

    uint64_t created_time;
    uint64_t last_updated_time;
    uint8_t is_active;
} Metric;

typedef struct
{
    uint32_t aggregation_id;
    uint32_t metric_id;

    uint64_t aggregation_window_ms;
    double aggregated_value;

    uint32_t sample_count_in_window;
    uint64_t aggregation_time;
} MetricAggregation;

typedef struct
{
    uint32_t collector_id;
    char collector_name[64];

    Metric metrics[MAX_METRICS];
    uint32_t metric_count;

    MetricAggregation aggregations[MAX_AGGREGATIONS];
    uint32_t aggregation_count;

    uint64_t total_samples_collected;
    uint64_t total_bytes_collected;
    uint32_t collection_interval_ms;
} MetricsCollector;

MetricsCollector *metrics_collector_init(uint32_t collection_interval_ms);
int metrics_collector_destroy(MetricsCollector *collector);

uint32_t metric_register(MetricsCollector *collector, const char *metric_name,
                         MetricType type, MetricSource source);
int metric_unregister(MetricsCollector *collector, uint32_t metric_id);

int metric_record_sample(MetricsCollector *collector, uint32_t metric_id,
                         double value, uint64_t timestamp);

int metric_get_current_value(MetricsCollector *collector, uint32_t metric_id, double *value);
int metric_get_statistics(MetricsCollector *collector, uint32_t metric_id,
                          double *min, double *max, double *avg);

uint32_t metric_aggregate(MetricsCollector *collector, uint32_t metric_id,
                          uint64_t window_ms);

int metric_aggregate_all(MetricsCollector *collector, uint64_t window_ms);

int metric_clear_samples(MetricsCollector *collector, uint32_t metric_id);
int metric_reset_statistics(MetricsCollector *collector, uint32_t metric_id);

int metric_export_samples(MetricsCollector *collector, uint32_t metric_id,
                          uint8_t *buffer, uint32_t *buffer_length);

typedef struct
{
    uint32_t collector_id;
    uint32_t total_metrics;
    uint32_t active_metrics;
    uint32_t total_samples;
    uint32_t total_aggregations;
    uint64_t total_bytes_collected;
    float collection_rate_hz;
} MetricsCollectorMetrics;

int metrics_collector_get_metrics(MetricsCollector *collector, MetricsCollectorMetrics *metrics);

#endif
