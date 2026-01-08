#ifndef ANALYTICS_ENGINE_H
#define ANALYTICS_ENGINE_H

#include <stdint.h>
#include <stddef.h>

#define MAX_ANALYTICS_MODELS 256
#define MAX_PREDICTIONS 4096
#define MAX_ANOMALIES 512
#define MAX_CORRELATIONS 1024
#define MAX_TRENDS 256

typedef enum
{
    MODEL_TYPE_LINEAR_REGRESSION,
    MODEL_TYPE_MOVING_AVERAGE,
    MODEL_TYPE_EXPONENTIAL_SMOOTHING,
    MODEL_TYPE_ARIMA,
    MODEL_TYPE_NEURAL_NETWORK
} AnalyticsModelType;

typedef enum
{
    ANOMALY_TYPE_STATISTICAL_OUTLIER,
    ANOMALY_TYPE_SEASONAL_ANOMALY,
    ANOMALY_TYPE_TREND_CHANGE,
    ANOMALY_TYPE_LEVEL_SHIFT,
    ANOMALY_TYPE_THRESHOLD_BREACH
} AnomalyType;

typedef struct
{
    uint32_t model_id;
    AnalyticsModelType model_type;
    char model_name[128];

    double *model_coefficients;
    uint32_t coefficient_count;

    uint64_t training_samples;
    double model_accuracy;
    double model_error_rate;

    uint64_t last_trained_time;
    uint8_t is_trained;
} AnalyticsModel;

typedef struct
{
    uint32_t prediction_id;
    uint32_t model_id;

    double predicted_value;
    double confidence_interval_lower;
    double confidence_interval_upper;

    uint64_t prediction_timestamp;
    uint64_t prediction_time;
} Prediction;

typedef struct
{
    uint32_t anomaly_id;
    uint32_t metric_id;
    AnomalyType anomaly_type;

    double observed_value;
    double expected_value;
    double deviation;

    uint64_t anomaly_timestamp;
    uint32_t consecutive_anomalies;
} Anomaly;

typedef struct
{
    uint32_t correlation_id;
    uint32_t metric_id_1;
    uint32_t metric_id_2;

    double correlation_coefficient;
    uint8_t is_significant;
} Correlation;

typedef struct
{
    uint32_t trend_id;
    uint32_t metric_id;

    double trend_slope;
    double trend_intercept;
    uint64_t trend_start_time;
    uint64_t trend_end_time;
} Trend;

typedef struct
{
    uint32_t engine_id;

    AnalyticsModel models[MAX_ANALYTICS_MODELS];
    uint32_t model_count;

    Prediction predictions[MAX_PREDICTIONS];
    uint32_t prediction_count;

    Anomaly anomalies[MAX_ANOMALIES];
    uint32_t anomaly_count;

    Correlation correlations[MAX_CORRELATIONS];
    uint32_t correlation_count;

    Trend trends[MAX_TRENDS];
    uint32_t trend_count;

    uint64_t analysis_operations;
    uint32_t anomalies_detected;
    double overall_model_accuracy;
} AnalyticsEngine;

AnalyticsEngine *analytics_engine_init(void);
int analytics_engine_destroy(AnalyticsEngine *engine);

uint32_t analytics_model_create(AnalyticsEngine *engine, AnalyticsModelType type,
                                const char *model_name);
int analytics_model_destroy(AnalyticsEngine *engine, uint32_t model_id);

int analytics_model_train(AnalyticsEngine *engine, uint32_t model_id,
                          const double *training_data, uint32_t data_count);

int analytics_model_predict(AnalyticsEngine *engine, uint32_t model_id,
                            const double *input_data, uint32_t input_count,
                            double *prediction, double *confidence_lower,
                            double *confidence_upper);

int analytics_generate_predictions(AnalyticsEngine *engine, uint32_t model_id,
                                   const double *data, uint32_t data_count,
                                   uint32_t forecast_periods);

int analytics_detect_anomalies(AnalyticsEngine *engine, uint32_t metric_id,
                               const double *data, uint32_t data_count,
                               double threshold);

int analytics_compute_correlations(AnalyticsEngine *engine, uint32_t metric_id_1,
                                   uint32_t metric_id_2, const double *data_1,
                                   const double *data_2, uint32_t data_count);

int analytics_detect_trends(AnalyticsEngine *engine, uint32_t metric_id,
                            const double *data, uint32_t data_count);

int analytics_get_anomaly_count(AnalyticsEngine *engine, uint32_t metric_id);
int analytics_get_prediction_count(AnalyticsEngine *engine, uint32_t model_id);

int analytics_anomaly_clear(AnalyticsEngine *engine);
int analytics_prediction_clear(AnalyticsEngine *engine);

int analytics_export_model(AnalyticsEngine *engine, uint32_t model_id,
                           uint8_t *buffer, uint32_t *buffer_length);

typedef struct
{
    uint32_t engine_id;
    uint32_t total_models;
    uint32_t trained_models;
    uint32_t total_predictions;
    uint32_t total_anomalies;
    uint32_t total_correlations;
    uint32_t total_trends;
    uint64_t analysis_operations;
    double average_model_accuracy;
} AnalyticsEngineMetrics;

int analytics_engine_get_metrics(AnalyticsEngine *engine, AnalyticsEngineMetrics *metrics);

#endif
