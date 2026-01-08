#ifndef ANALYTICS_ENGINE_H
#define ANALYTICS_ENGINE_H

#include <stdint.h>
#include <stddef.h>

#define MAX_ANALYTICS_ENGINES 16
#define MAX_METRICS_STREAMS 256
#define MAX_AGGREGATIONS 1024
#define MAX_DASHBOARDS 64
#define MAX_ALERTS 512
#define MAX_ANALYTICS_RULES 128
#define MAX_TIME_WINDOWS 16

typedef enum
{
    WINDOW_TYPE_TUMBLING = 1,
    WINDOW_TYPE_SLIDING = 2,
    WINDOW_TYPE_SESSION = 3
} WindowType;

typedef enum
{
    METRIC_COUNT = 1,
    METRIC_SUM = 2,
    METRIC_AVG = 3,
    METRIC_MIN = 4,
    METRIC_MAX = 5,
    METRIC_STDDEV = 6,
    METRIC_PERCENTILE = 7
} AggregationType;

typedef enum
{
    ALERT_THRESHOLD = 1,
    ALERT_ANOMALY = 2,
    ALERT_TREND = 3
} AlertType;

typedef struct
{
    uint64_t timestamp;
    float value;
    uint32_t source_id;
} MetricDataPoint;

typedef struct
{
    uint32_t stream_id;
    char stream_name[64];
    uint32_t source_id;

    MetricDataPoint data_points[4096];
    uint32_t data_point_count;

    float min_value;
    float max_value;
    float current_value;
    uint64_t last_update;
} MetricsStream;

typedef struct
{
    uint32_t aggregation_id;
    char aggregation_name[64];
    uint32_t stream_id;

    AggregationType agg_type;
    uint32_t window_size_seconds;
    WindowType window_type;

    float aggregated_value;
    uint64_t last_computed;
    uint32_t data_point_count_in_window;

    float *historical_values;
    uint32_t historical_count;
} Aggregation;

typedef struct
{
    uint32_t alert_id;
    AlertType alert_type;
    uint32_t aggregation_id;
    float threshold_value;
    uint32_t is_enabled;
    uint32_t is_triggered;
    uint64_t trigger_count;
} AnalyticsAlert;

typedef struct
{
    uint32_t rule_id;
    char rule_name[64];
    uint32_t stream_id;
    char metric_expression[256];
    uint32_t is_enabled;
} AnalyticsRule;

typedef struct
{
    uint32_t dashboard_id;
    char dashboard_name[128];
    uint32_t aggregation_ids[64];
    uint32_t aggregation_count;
    uint64_t created_at;
    uint64_t last_updated;
} AnalyticsDashboard;

typedef struct
{
    uint32_t engine_id;
    char name[64];

    MetricsStream streams[MAX_METRICS_STREAMS];
    uint32_t stream_count;

    Aggregation aggregations[MAX_AGGREGATIONS];
    uint32_t aggregation_count;

    AnalyticsAlert alerts[MAX_ALERTS];
    uint32_t alert_count;

    AnalyticsRule rules[MAX_ANALYTICS_RULES];
    uint32_t rule_count;

    AnalyticsDashboard dashboards[MAX_DASHBOARDS];
    uint32_t dashboard_count;

    uint32_t real_time_enabled;
    uint32_t batch_enabled;
    uint32_t ml_enabled;

    uint64_t total_data_points_processed;
    uint64_t total_aggregations_computed;
    uint64_t total_alerts_triggered;
    uint64_t bytes_processed;
} AnalyticsEngine;

int ae_create_engine(const char *name);
int ae_delete_engine(uint32_t engine_id);

int ae_create_metrics_stream(uint32_t engine_id, const char *stream_name, uint32_t source_id);
int ae_delete_stream(uint32_t engine_id, uint32_t stream_id);
int ae_feed_metric_data(uint32_t engine_id, uint32_t stream_id, float value, uint64_t timestamp);
int ae_feed_batch_metrics(uint32_t engine_id, uint32_t stream_id, float *values,
                          uint64_t *timestamps, uint32_t count);

int ae_create_aggregation(uint32_t engine_id, uint32_t stream_id, const char *agg_name,
                          AggregationType agg_type, uint32_t window_size_sec, WindowType window_type);
int ae_delete_aggregation(uint32_t engine_id, uint32_t agg_id);
int ae_compute_aggregation(uint32_t engine_id, uint32_t agg_id);
int ae_compute_all_aggregations(uint32_t engine_id);

int ae_add_alert(uint32_t engine_id, uint32_t agg_id, AlertType alert_type, float threshold);
int ae_remove_alert(uint32_t engine_id, uint32_t alert_id);
int ae_evaluate_alerts(uint32_t engine_id);
int ae_get_triggered_alerts(uint32_t engine_id, AnalyticsAlert *alerts, uint32_t *count);

int ae_add_analytics_rule(uint32_t engine_id, uint32_t stream_id, const char *rule_name,
                          const char *metric_expression);
int ae_remove_analytics_rule(uint32_t engine_id, uint32_t rule_id);
int ae_evaluate_rules(uint32_t engine_id);

int ae_create_dashboard(uint32_t engine_id, const char *dashboard_name);
int ae_delete_dashboard(uint32_t engine_id, uint32_t dashboard_id);
int ae_add_aggregation_to_dashboard(uint32_t engine_id, uint32_t dashboard_id, uint32_t agg_id);
int ae_remove_aggregation_from_dashboard(uint32_id engine_id, uint32_t dashboard_id, uint32_t agg_id);

int ae_get_stream_statistics(uint32_t engine_id, uint32_t stream_id,
                             float *min_val, float *max_val, float *avg_val);
int ae_get_aggregation_value(uint32_t engine_id, uint32_t agg_id, float *value);
int ae_get_aggregation_history(uint32_t engine_id, uint32_t agg_id,
                               float *values, uint32_t *count);

int ae_enable_real_time_processing(uint32_t engine_id, uint32_t enable);
int ae_enable_batch_processing(uint32_t engine_id, uint32_t enable);
int ae_enable_ml_analysis(uint32_t engine_id, uint32_t enable);

uint64_t ae_get_data_points_processed(uint32_t engine_id);
uint64_t ae_get_aggregations_computed(uint32_t engine_id);
uint64_t ae_get_alerts_triggered(uint32_t engine_id);

int ae_export_metrics(uint32_t engine_id, const char *format);
int ae_export_dashboard(uint32_t engine_id, uint32_t dashboard_id, const char *format);

AnalyticsEngine *ae_get_engine_info(uint32_t engine_id);
MetricsStream *ae_get_stream_info(uint32_t engine_id, uint32_t stream_id);

#endif
