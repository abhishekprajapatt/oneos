#include "metrics_collector.h"
#include <stdlib.h>
#include <string.h>

static MetricsCollector *global_metrics_collector = NULL;
static uint32_t next_metric_id = 1;
static uint32_t next_sample_id = 1;
static uint32_t next_aggregation_id = 1;

MetricsCollector *metrics_collector_init(uint32_t collection_interval_ms)
{
    MetricsCollector *collector = (MetricsCollector *)malloc(sizeof(MetricsCollector));
    if (!collector)
        return NULL;

    memset(collector, 0, sizeof(MetricsCollector));
    collector->collector_id = 1;
    collector->metric_count = 0;
    collector->aggregation_count = 0;
    collector->total_samples_collected = 0;
    collector->total_bytes_collected = 0;
    collector->collection_interval_ms = collection_interval_ms;

    global_metrics_collector = collector;
    return collector;
}

int metrics_collector_destroy(MetricsCollector *collector)
{
    if (!collector)
        return -1;

    free(collector);
    global_metrics_collector = NULL;
    return 0;
}

uint32_t metric_register(MetricsCollector *collector, const char *metric_name,
                         MetricType type, MetricSource source)
{
    if (!collector || collector->metric_count >= MAX_METRICS || !metric_name)
        return 0;

    Metric *metric = &collector->metrics[collector->metric_count];
    memset(metric, 0, sizeof(Metric));

    metric->metric_id = next_metric_id++;
    strncpy(metric->metric_name, metric_name, sizeof(metric->metric_name) - 1);
    metric->metric_type = type;
    metric->source = source;
    metric->sample_count = 0;

    metric->current_value = 0.0;
    metric->min_value = 1e9;
    metric->max_value = 0.0;
    metric->average_value = 0.0;

    metric->created_time = 0;
    metric->last_updated_time = 0;
    metric->is_active = 1;

    collector->metric_count++;
    return metric->metric_id;
}

int metric_unregister(MetricsCollector *collector, uint32_t metric_id)
{
    if (!collector || metric_id == 0)
        return -1;

    for (uint32_t i = 0; i < collector->metric_count; i++)
    {
        if (collector->metrics[i].metric_id == metric_id)
        {
            memmove(&collector->metrics[i], &collector->metrics[i + 1],
                    sizeof(Metric) * (collector->metric_count - i - 1));
            collector->metric_count--;
            return 0;
        }
    }

    return -1;
}

int metric_record_sample(MetricsCollector *collector, uint32_t metric_id,
                         double value, uint64_t timestamp)
{
    if (!collector || metric_id == 0)
        return -1;

    for (uint32_t i = 0; i < collector->metric_count; i++)
    {
        if (collector->metrics[i].metric_id == metric_id && collector->metrics[i].is_active)
        {
            Metric *metric = &collector->metrics[i];

            if (metric->sample_count >= MAX_METRIC_SAMPLES)
                return -1;

            MetricSample *sample = &metric->samples[metric->sample_count];
            sample->timestamp = timestamp;
            sample->value = value;
            sample->sample_id = next_sample_id++;

            metric->current_value = value;
            if (value < metric->min_value)
                metric->min_value = value;
            if (value > metric->max_value)
                metric->max_value = value;

            metric->average_value = (metric->average_value * metric->sample_count + value) / (metric->sample_count + 1);

            metric->sample_count++;
            metric->last_updated_time = timestamp;

            collector->total_samples_collected++;
            collector->total_bytes_collected += sizeof(MetricSample);

            return 0;
        }
    }

    return -1;
}

int metric_get_current_value(MetricsCollector *collector, uint32_t metric_id, double *value)
{
    if (!collector || metric_id == 0 || !value)
        return -1;

    for (uint32_t i = 0; i < collector->metric_count; i++)
    {
        if (collector->metrics[i].metric_id == metric_id)
        {
            *value = collector->metrics[i].current_value;
            return 0;
        }
    }

    return -1;
}

int metric_get_statistics(MetricsCollector *collector, uint32_t metric_id,
                          double *min, double *max, double *avg)
{
    if (!collector || metric_id == 0 || !min || !max || !avg)
        return -1;

    for (uint32_t i = 0; i < collector->metric_count; i++)
    {
        if (collector->metrics[i].metric_id == metric_id)
        {
            Metric *metric = &collector->metrics[i];
            *min = metric->min_value;
            *max = metric->max_value;
            *avg = metric->average_value;
            return 0;
        }
    }

    return -1;
}

uint32_t metric_aggregate(MetricsCollector *collector, uint32_t metric_id,
                          uint64_t window_ms)
{
    if (!collector || metric_id == 0 || collector->aggregation_count >= MAX_AGGREGATIONS)
        return 0;

    for (uint32_t i = 0; i < collector->metric_count; i++)
    {
        if (collector->metrics[i].metric_id == metric_id)
        {
            MetricAggregation *agg = &collector->aggregations[collector->aggregation_count];
            memset(agg, 0, sizeof(MetricAggregation));

            agg->aggregation_id = next_aggregation_id++;
            agg->metric_id = metric_id;
            agg->aggregation_window_ms = window_ms;
            agg->aggregated_value = collector->metrics[i].average_value;
            agg->sample_count_in_window = collector->metrics[i].sample_count;
            agg->aggregation_time = 0;

            collector->aggregation_count++;
            return agg->aggregation_id;
        }
    }

    return 0;
}

int metric_aggregate_all(MetricsCollector *collector, uint64_t window_ms)
{
    if (!collector)
        return -1;

    for (uint32_t i = 0; i < collector->metric_count; i++)
    {
        if (collector->metrics[i].is_active)
        {
            metric_aggregate(collector, collector->metrics[i].metric_id, window_ms);
        }
    }

    return 0;
}

int metric_clear_samples(MetricsCollector *collector, uint32_t metric_id)
{
    if (!collector || metric_id == 0)
        return -1;

    for (uint32_t i = 0; i < collector->metric_count; i++)
    {
        if (collector->metrics[i].metric_id == metric_id)
        {
            Metric *metric = &collector->metrics[i];
            metric->sample_count = 0;
            return 0;
        }
    }

    return -1;
}

int metric_reset_statistics(MetricsCollector *collector, uint32_t metric_id)
{
    if (!collector || metric_id == 0)
        return -1;

    for (uint32_t i = 0; i < collector->metric_count; i++)
    {
        if (collector->metrics[i].metric_id == metric_id)
        {
            Metric *metric = &collector->metrics[i];
            metric->min_value = 1e9;
            metric->max_value = 0.0;
            metric->average_value = 0.0;
            metric->sample_count = 0;
            return 0;
        }
    }

    return -1;
}

int metric_export_samples(MetricsCollector *collector, uint32_t metric_id,
                          uint8_t *buffer, uint32_t *buffer_length)
{
    if (!collector || metric_id == 0 || !buffer || !buffer_length)
        return -1;

    for (uint32_t i = 0; i < collector->metric_count; i++)
    {
        if (collector->metrics[i].metric_id == metric_id)
        {
            Metric *metric = &collector->metrics[i];
            uint32_t required_size = metric->sample_count * sizeof(MetricSample);

            if (required_size <= *buffer_length)
            {
                memcpy(buffer, metric->samples, required_size);
                *buffer_length = required_size;
                return 0;
            }

            return -1;
        }
    }

    return -1;
}

int metrics_collector_get_metrics(MetricsCollector *collector, MetricsCollectorMetrics *metrics)
{
    if (!collector || !metrics)
        return -1;

    uint32_t active_metrics = 0;

    for (uint32_t i = 0; i < collector->metric_count; i++)
    {
        if (collector->metrics[i].is_active)
        {
            active_metrics++;
        }
    }

    metrics->collector_id = collector->collector_id;
    metrics->total_metrics = collector->metric_count;
    metrics->active_metrics = active_metrics;
    metrics->total_samples = (uint32_t)collector->total_samples_collected;
    metrics->total_aggregations = collector->aggregation_count;
    metrics->total_bytes_collected = collector->total_bytes_collected;
    metrics->collection_rate_hz = collector->collection_interval_ms > 0 ? 1000.0f / collector->collection_interval_ms : 0.0f;

    return 0;
}
