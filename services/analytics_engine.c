#include "analytics_engine.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static AnalyticsEngine g_engines[MAX_ANALYTICS_ENGINES] = {0};
static uint32_t g_engine_count = 0;

static float compute_mean(float *values, uint32_t count)
{
    if (count == 0)
        return 0.0f;
    float sum = 0.0f;
    for (uint32_t i = 0; i < count; i++)
    {
        sum += values[i];
    }
    return sum / count;
}

static float compute_stddev(float *values, uint32_t count, float mean)
{
    if (count <= 1)
        return 0.0f;
    float variance = 0.0f;
    for (uint32_t i = 0; i < count; i++)
    {
        float diff = values[i] - mean;
        variance += diff * diff;
    }
    return sqrtf(variance / (count - 1));
}

int ae_create_engine(const char *name)
{
    if (!name || g_engine_count >= MAX_ANALYTICS_ENGINES)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[g_engine_count];
    engine->engine_id = g_engine_count + 1;
    strncpy(engine->name, name, 63);

    engine->stream_count = 0;
    engine->aggregation_count = 0;
    engine->alert_count = 0;
    engine->rule_count = 0;
    engine->dashboard_count = 0;

    engine->real_time_enabled = 1;
    engine->batch_enabled = 1;
    engine->ml_enabled = 1;

    engine->total_data_points_processed = 0;
    engine->total_aggregations_computed = 0;
    engine->total_alerts_triggered = 0;
    engine->bytes_processed = 0;

    g_engine_count++;
    return engine->engine_id;
}

int ae_delete_engine(uint32_t engine_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    for (uint32_t i = engine_id - 1; i < g_engine_count - 1; i++)
    {
        g_engines[i] = g_engines[i + 1];
    }
    g_engine_count--;

    return 0;
}

int ae_create_metrics_stream(uint32_t engine_id, const char *stream_name, uint32_t source_id)
{
    if (engine_id == 0 || engine_id > g_engine_count || !stream_name)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];
    if (engine->stream_count >= MAX_METRICS_STREAMS)
    {
        return -1;
    }

    MetricsStream *stream = &engine->streams[engine->stream_count];
    stream->stream_id = engine->stream_count + 1;
    strncpy(stream->stream_name, stream_name, 63);
    stream->source_id = source_id;
    stream->data_point_count = 0;
    stream->min_value = 0.0f;
    stream->max_value = 0.0f;
    stream->current_value = 0.0f;
    stream->last_update = 0;

    engine->stream_count++;

    return stream->stream_id;
}

int ae_delete_stream(uint32_t engine_id, uint32_t stream_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->stream_count; i++)
    {
        if (engine->streams[i].stream_id == stream_id)
        {
            for (uint32_t j = i; j < engine->stream_count - 1; j++)
            {
                engine->streams[j] = engine->streams[j + 1];
            }
            engine->stream_count--;
            return 0;
        }
    }

    return -1;
}

int ae_feed_metric_data(uint32_t engine_id, uint32_t stream_id, float value, uint64_t timestamp)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->stream_count; i++)
    {
        if (engine->streams[i].stream_id == stream_id)
        {
            MetricsStream *stream = &engine->streams[i];

            if (stream->data_point_count >= 4096)
            {
                for (uint32_t j = 0; j < 4095; j++)
                {
                    stream->data_points[j] = stream->data_points[j + 1];
                }
                stream->data_point_count = 4095;
            }

            MetricDataPoint *dp = &stream->data_points[stream->data_point_count];
            dp->timestamp = timestamp;
            dp->value = value;
            dp->source_id = stream->source_id;

            stream->data_point_count++;
            stream->current_value = value;
            stream->last_update = timestamp;

            if (stream->data_point_count == 1)
            {
                stream->min_value = value;
                stream->max_value = value;
            }
            else
            {
                if (value < stream->min_value)
                    stream->min_value = value;
                if (value > stream->max_value)
                    stream->max_value = value;
            }

            engine->total_data_points_processed++;
            engine->bytes_processed += sizeof(MetricDataPoint);

            return 0;
        }
    }

    return -1;
}

int ae_feed_batch_metrics(uint32_t engine_id, uint32_t stream_id, float *values,
                          uint64_t *timestamps, uint32_t count)
{
    if (engine_id == 0 || engine_id > g_engine_count || !values || !timestamps)
    {
        return -1;
    }

    for (uint32_t i = 0; i < count; i++)
    {
        if (ae_feed_metric_data(engine_id, stream_id, values[i], timestamps[i]) != 0)
        {
            return -1;
        }
    }

    return 0;
}

int ae_create_aggregation(uint32_t engine_id, uint32_t stream_id, const char *agg_name,
                          AggregationType agg_type, uint32_t window_size_sec, WindowType window_type)
{
    if (engine_id == 0 || engine_id > g_engine_count || !agg_name)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];
    if (engine->aggregation_count >= MAX_AGGREGATIONS)
    {
        return -1;
    }

    Aggregation *agg = &engine->aggregations[engine->aggregation_count];
    agg->aggregation_id = engine->aggregation_count + 1;
    strncpy(agg->aggregation_name, agg_name, 63);
    agg->stream_id = stream_id;
    agg->agg_type = agg_type;
    agg->window_size_seconds = window_size_sec;
    agg->window_type = window_type;
    agg->aggregated_value = 0.0f;
    agg->last_computed = 0;
    agg->data_point_count_in_window = 0;
    agg->historical_values = NULL;
    agg->historical_count = 0;

    engine->aggregation_count++;

    return agg->aggregation_id;
}

int ae_delete_aggregation(uint32_t engine_id, uint32_t agg_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->aggregation_count; i++)
    {
        if (engine->aggregations[i].aggregation_id == agg_id)
        {
            if (engine->aggregations[i].historical_values)
            {
                free(engine->aggregations[i].historical_values);
            }

            for (uint32_t j = i; j < engine->aggregation_count - 1; j++)
            {
                engine->aggregations[j] = engine->aggregations[j + 1];
            }
            engine->aggregation_count--;
            return 0;
        }
    }

    return -1;
}

int ae_compute_aggregation(uint32_t engine_id, uint32_t agg_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->aggregation_count; i++)
    {
        if (engine->aggregations[i].aggregation_id == agg_id)
        {
            Aggregation *agg = &engine->aggregations[i];

            MetricsStream *stream = NULL;
            for (uint32_t j = 0; j < engine->stream_count; j++)
            {
                if (engine->streams[j].stream_id == agg->stream_id)
                {
                    stream = &engine->streams[j];
                    break;
                }
            }

            if (!stream || stream->data_point_count == 0)
            {
                return -1;
            }

            uint32_t start_idx = (stream->data_point_count > 100) ? stream->data_point_count - 100 : 0;
            uint32_t window_size = stream->data_point_count - start_idx;

            switch (agg->agg_type)
            {
            case METRIC_COUNT:
                agg->aggregated_value = (float)window_size;
                break;
            case METRIC_SUM:
            {
                float sum = 0.0f;
                for (uint32_t j = start_idx; j < stream->data_point_count; j++)
                {
                    sum += stream->data_points[j].value;
                }
                agg->aggregated_value = sum;
                break;
            }
            case METRIC_AVG:
            {
                float sum = 0.0f;
                for (uint32_t j = start_idx; j < stream->data_point_count; j++)
                {
                    sum += stream->data_points[j].value;
                }
                agg->aggregated_value = sum / window_size;
                break;
            }
            case METRIC_MIN:
            {
                float min_val = stream->data_points[start_idx].value;
                for (uint32_t j = start_idx; j < stream->data_point_count; j++)
                {
                    if (stream->data_points[j].value < min_val)
                    {
                        min_val = stream->data_points[j].value;
                    }
                }
                agg->aggregated_value = min_val;
                break;
            }
            case METRIC_MAX:
            {
                float max_val = stream->data_points[start_idx].value;
                for (uint32_t j = start_idx; j < stream->data_point_count; j++)
                {
                    if (stream->data_points[j].value > max_val)
                    {
                        max_val = stream->data_points[j].value;
                    }
                }
                agg->aggregated_value = max_val;
                break;
            }
            case METRIC_STDDEV:
            {
                float values[256];
                uint32_t count = (window_size > 256) ? 256 : window_size;
                for (uint32_t j = 0; j < count; j++)
                {
                    values[j] = stream->data_points[start_idx + j].value;
                }
                float mean = compute_mean(values, count);
                agg->aggregated_value = compute_stddev(values, count, mean);
                break;
            }
            case METRIC_PERCENTILE:
                agg->aggregated_value = stream->data_points[stream->data_point_count - 1].value;
                break;
            }

            agg->last_computed = 0;
            agg->data_point_count_in_window = window_size;

            engine->total_aggregations_computed++;

            return 0;
        }
    }

    return -1;
}

int ae_compute_all_aggregations(uint32_t engine_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->aggregation_count; i++)
    {
        if (ae_compute_aggregation(engine_id, engine->aggregations[i].aggregation_id) != 0)
        {
            return -1;
        }
    }

    return 0;
}

int ae_add_alert(uint32_t engine_id, uint32_t agg_id, AlertType alert_type, float threshold)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];
    if (engine->alert_count >= MAX_ALERTS)
    {
        return -1;
    }

    AnalyticsAlert *alert = &engine->alerts[engine->alert_count];
    alert->alert_id = engine->alert_count + 1;
    alert->alert_type = alert_type;
    alert->aggregation_id = agg_id;
    alert->threshold_value = threshold;
    alert->is_enabled = 1;
    alert->is_triggered = 0;
    alert->trigger_count = 0;

    engine->alert_count++;

    return alert->alert_id;
}

int ae_remove_alert(uint32_t engine_id, uint32_t alert_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->alert_count; i++)
    {
        if (engine->alerts[i].alert_id == alert_id)
        {
            for (uint32_t j = i; j < engine->alert_count - 1; j++)
            {
                engine->alerts[j] = engine->alerts[j + 1];
            }
            engine->alert_count--;
            return 0;
        }
    }

    return -1;
}

int ae_evaluate_alerts(uint32_t engine_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->alert_count; i++)
    {
        if (!engine->alerts[i].is_enabled)
        {
            continue;
        }

        for (uint32_t j = 0; j < engine->aggregation_count; j++)
        {
            if (engine->aggregations[j].aggregation_id == engine->alerts[i].aggregation_id)
            {
                float agg_value = engine->aggregations[j].aggregated_value;

                if (agg_value > engine->alerts[i].threshold_value)
                {
                    engine->alerts[i].is_triggered = 1;
                    engine->alerts[i].trigger_count++;
                    engine->total_alerts_triggered++;
                }
                else
                {
                    engine->alerts[i].is_triggered = 0;
                }
                break;
            }
        }
    }

    return 0;
}

int ae_get_triggered_alerts(uint32_t engine_id, AnalyticsAlert *alerts, uint32_t *count)
{
    if (engine_id == 0 || engine_id > g_engine_count || !alerts || !count)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];
    uint32_t alert_count = 0;

    for (uint32_t i = 0; i < engine->alert_count && alert_count < *count; i++)
    {
        if (engine->alerts[i].is_triggered)
        {
            alerts[alert_count++] = engine->alerts[i];
        }
    }

    *count = alert_count;
    return 0;
}

int ae_add_analytics_rule(uint32_t engine_id, uint32_t stream_id, const char *rule_name,
                          const char *metric_expression)
{
    if (engine_id == 0 || engine_id > g_engine_count || !rule_name || !metric_expression)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];
    if (engine->rule_count >= MAX_ANALYTICS_RULES)
    {
        return -1;
    }

    AnalyticsRule *rule = &engine->rules[engine->rule_count];
    rule->rule_id = engine->rule_count + 1;
    strncpy(rule->rule_name, rule_name, 63);
    rule->stream_id = stream_id;
    strncpy(rule->metric_expression, metric_expression, 255);
    rule->is_enabled = 1;

    engine->rule_count++;

    return rule->rule_id;
}

int ae_remove_analytics_rule(uint32_t engine_id, uint32_t rule_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->rule_count; i++)
    {
        if (engine->rules[i].rule_id == rule_id)
        {
            for (uint32_t j = i; j < engine->rule_count - 1; j++)
            {
                engine->rules[j] = engine->rules[j + 1];
            }
            engine->rule_count--;
            return 0;
        }
    }

    return -1;
}

int ae_evaluate_rules(uint32_t engine_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    return 0;
}

int ae_create_dashboard(uint32_t engine_id, const char *dashboard_name)
{
    if (engine_id == 0 || engine_id > g_engine_count || !dashboard_name)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];
    if (engine->dashboard_count >= MAX_DASHBOARDS)
    {
        return -1;
    }

    AnalyticsDashboard *dashboard = &engine->dashboards[engine->dashboard_count];
    dashboard->dashboard_id = engine->dashboard_count + 1;
    strncpy(dashboard->dashboard_name, dashboard_name, 127);
    dashboard->aggregation_count = 0;
    dashboard->created_at = 0;
    dashboard->last_updated = 0;

    engine->dashboard_count++;

    return dashboard->dashboard_id;
}

int ae_delete_dashboard(uint32_t engine_id, uint32_t dashboard_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->dashboard_count; i++)
    {
        if (engine->dashboards[i].dashboard_id == dashboard_id)
        {
            for (uint32_t j = i; j < engine->dashboard_count - 1; j++)
            {
                engine->dashboards[j] = engine->dashboards[j + 1];
            }
            engine->dashboard_count--;
            return 0;
        }
    }

    return -1;
}

int ae_add_aggregation_to_dashboard(uint32_t engine_id, uint32_t dashboard_id, uint32_t agg_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->dashboard_count; i++)
    {
        if (engine->dashboards[i].dashboard_id == dashboard_id)
        {
            if (engine->dashboards[i].aggregation_count >= 64)
            {
                return -1;
            }

            engine->dashboards[i].aggregation_ids[engine->dashboards[i].aggregation_count++] = agg_id;
            engine->dashboards[i].last_updated = 0;
            return 0;
        }
    }

    return -1;
}

int ae_remove_aggregation_from_dashboard(uint32_id engine_id, uint32_t dashboard_id, uint32_t agg_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->dashboard_count; i++)
    {
        if (engine->dashboards[i].dashboard_id == dashboard_id)
        {
            for (uint32_t j = 0; j < engine->dashboards[i].aggregation_count; j++)
            {
                if (engine->dashboards[i].aggregation_ids[j] == agg_id)
                {
                    for (uint32_t k = j; k < engine->dashboards[i].aggregation_count - 1; k++)
                    {
                        engine->dashboards[i].aggregation_ids[k] =
                            engine->dashboards[i].aggregation_ids[k + 1];
                    }
                    engine->dashboards[i].aggregation_count--;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int ae_get_stream_statistics(uint32_t engine_id, uint32_t stream_id,
                             float *min_val, float *max_val, float *avg_val)
{
    if (engine_id == 0 || engine_id > g_engine_count || !min_val || !max_val || !avg_val)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->stream_count; i++)
    {
        if (engine->streams[i].stream_id == stream_id)
        {
            MetricsStream *stream = &engine->streams[i];

            *min_val = stream->min_value;
            *max_val = stream->max_value;

            float sum = 0.0f;
            for (uint32_t j = 0; j < stream->data_point_count; j++)
            {
                sum += stream->data_points[j].value;
            }
            *avg_val = (stream->data_point_count > 0) ? sum / stream->data_point_count : 0.0f;

            return 0;
        }
    }

    return -1;
}

int ae_get_aggregation_value(uint32_t engine_id, uint32_t agg_id, float *value)
{
    if (engine_id == 0 || engine_id > g_engine_count || !value)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->aggregation_count; i++)
    {
        if (engine->aggregations[i].aggregation_id == agg_id)
        {
            *value = engine->aggregations[i].aggregated_value;
            return 0;
        }
    }

    return -1;
}

int ae_get_aggregation_history(uint32_t engine_id, uint32_t agg_id,
                               float *values, uint32_t *count)
{
    if (engine_id == 0 || engine_id > g_engine_count || !values || !count)
    {
        return -1;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->aggregation_count; i++)
    {
        if (engine->aggregations[i].aggregation_id == agg_id)
        {
            Aggregation *agg = &engine->aggregations[i];

            if (agg->historical_count > *count)
            {
                return -1;
            }

            if (agg->historical_values && agg->historical_count > 0)
            {
                memcpy(values, agg->historical_values, agg->historical_count * sizeof(float));
                *count = agg->historical_count;
            }
            else
            {
                *count = 0;
            }

            return 0;
        }
    }

    return -1;
}

int ae_enable_real_time_processing(uint32_t engine_id, uint32_t enable)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    g_engines[engine_id - 1].real_time_enabled = enable;
    return 0;
}

int ae_enable_batch_processing(uint32_t engine_id, uint32_t enable)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    g_engines[engine_id - 1].batch_enabled = enable;
    return 0;
}

int ae_enable_ml_analysis(uint32_t engine_id, uint32_t enable)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return -1;
    }

    g_engines[engine_id - 1].ml_enabled = enable;
    return 0;
}

uint64_t ae_get_data_points_processed(uint32_t engine_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return 0;
    }

    return g_engines[engine_id - 1].total_data_points_processed;
}

uint64_t ae_get_aggregations_computed(uint32_t engine_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return 0;
    }

    return g_engines[engine_id - 1].total_aggregations_computed;
}

uint64_t ae_get_alerts_triggered(uint32_t engine_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return 0;
    }

    return g_engines[engine_id - 1].total_alerts_triggered;
}

int ae_export_metrics(uint32_t engine_id, const char *format)
{
    if (engine_id == 0 || engine_id > g_engine_count || !format)
    {
        return -1;
    }

    return 0;
}

int ae_export_dashboard(uint32_t engine_id, uint32_t dashboard_id, const char *format)
{
    if (engine_id == 0 || engine_id > g_engine_count || !format)
    {
        return -1;
    }

    return 0;
}

AnalyticsEngine *ae_get_engine_info(uint32_t engine_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return NULL;
    }

    return &g_engines[engine_id - 1];
}

MetricsStream *ae_get_stream_info(uint32_t engine_id, uint32_t stream_id)
{
    if (engine_id == 0 || engine_id > g_engine_count)
    {
        return NULL;
    }

    AnalyticsEngine *engine = &g_engines[engine_id - 1];

    for (uint32_t i = 0; i < engine->stream_count; i++)
    {
        if (engine->streams[i].stream_id == stream_id)
        {
            return &engine->streams[i];
        }
    }

    return NULL;
}
