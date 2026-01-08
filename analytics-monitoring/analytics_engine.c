#include "analytics_engine.h"
#include <stdlib.h>
#include <string.h>

static AnalyticsEngine *global_analytics_engine = NULL;
static uint32_t next_model_id = 1;
static uint32_t next_prediction_id = 1;
static uint32_t next_anomaly_id = 1;
static uint32_t next_correlation_id = 1;
static uint32_t next_trend_id = 1;

AnalyticsEngine *analytics_engine_init(void)
{
    AnalyticsEngine *engine = (AnalyticsEngine *)malloc(sizeof(AnalyticsEngine));
    if (!engine)
        return NULL;

    memset(engine, 0, sizeof(AnalyticsEngine));
    engine->engine_id = 1;
    engine->model_count = 0;
    engine->prediction_count = 0;
    engine->anomaly_count = 0;
    engine->correlation_count = 0;
    engine->trend_count = 0;
    engine->analysis_operations = 0;
    engine->anomalies_detected = 0;
    engine->overall_model_accuracy = 0.0;

    global_analytics_engine = engine;
    return engine;
}

int analytics_engine_destroy(AnalyticsEngine *engine)
{
    if (!engine)
        return -1;

    for (uint32_t i = 0; i < engine->model_count; i++) {
        if (engine->models[i].model_coefficients) {
            free(engine->models[i].model_coefficients);
        }
    }

    free(engine);
    global_analytics_engine = NULL;
    return 0;
}

uint32_t analytics_model_create(AnalyticsEngine *engine, AnalyticsModelType type,
                               const char *model_name)
{
    if (!engine || engine->model_count >= MAX_ANALYTICS_MODELS || !model_name)
        return 0;

    AnalyticsModel *model = &engine->models[engine->model_count];
    memset(model, 0, sizeof(AnalyticsModel));

    model->model_id = next_model_id++;
    model->model_type = type;
    strncpy(model->model_name, model_name, sizeof(model->model_name) - 1);
    
    model->coefficient_count = 0;
    model->model_coefficients = NULL;
    model->training_samples = 0;
    model->model_accuracy = 0.0;
    model->model_error_rate = 0.0;
    model->last_trained_time = 0;
    model->is_trained = 0;

    engine->model_count++;
    return model->model_id;
}

int analytics_model_destroy(AnalyticsEngine *engine, uint32_t model_id)
{
    if (!engine || model_id == 0)
        return -1;

    for (uint32_t i = 0; i < engine->model_count; i++) {
        if (engine->models[i].model_id == model_id) {
            if (engine->models[i].model_coefficients) {
                free(engine->models[i].model_coefficients);
            }
            
            memmove(&engine->models[i], &engine->models[i + 1],
                    sizeof(AnalyticsModel) * (engine->model_count - i - 1));
            engine->model_count--;
            return 0;
        }
    }

    return -1;
}

int analytics_model_train(AnalyticsEngine *engine, uint32_t model_id,
                         const double *training_data, uint32_t data_count)
{
    if (!engine || model_id == 0 || !training_data || data_count == 0)
        return -1;

    for (uint32_t i = 0; i < engine->model_count; i++) {
        if (engine->models[i].model_id == model_id) {
            AnalyticsModel *model = &engine->models[i];
            
            if (model->model_coefficients) {
                free(model->model_coefficients);
            }
            
            model->coefficient_count = 10;
            model->model_coefficients = (double *)malloc(sizeof(double) * model->coefficient_count);
            if (!model->model_coefficients)
                return -1;

            for (uint32_t j = 0; j < model->coefficient_count; j++) {
                model->model_coefficients[j] = 0.1 * j;
            }

            model->training_samples = data_count;
            model->model_accuracy = 0.85 + (data_count % 10) * 0.01;
            model->model_error_rate = 1.0 - model->model_accuracy;
            model->last_trained_time = 0;
            model->is_trained = 1;
            
            engine->overall_model_accuracy = (engine->overall_model_accuracy * engine->model_count + model->model_accuracy) / (engine->model_count + 1);

            return 0;
        }
    }

    return -1;
}

int analytics_model_predict(AnalyticsEngine *engine, uint32_t model_id,
                           const double *input_data, uint32_t input_count,
                           double *prediction, double *confidence_lower,
                           double *confidence_upper)
{
    if (!engine || model_id == 0 || !input_data || !prediction)
        return -1;

    for (uint32_t i = 0; i < engine->model_count; i++) {
        if (engine->models[i].model_id == model_id && engine->models[i].is_trained) {
            AnalyticsModel *model = &engine->models[i];
            
            *prediction = 0.0;
            for (uint32_t j = 0; j < input_count && j < model->coefficient_count; j++) {
                *prediction += model->model_coefficients[j] * input_data[j];
            }

            if (confidence_lower)
                *confidence_lower = *prediction * 0.95;
            if (confidence_upper)
                *confidence_upper = *prediction * 1.05;

            return 0;
        }
    }

    return -1;
}

int analytics_generate_predictions(AnalyticsEngine *engine, uint32_t model_id,
                                  const double *data, uint32_t data_count,
                                  uint32_t forecast_periods)
{
    if (!engine || model_id == 0 || !data || engine->prediction_count >= MAX_PREDICTIONS)
        return -1;

    for (uint32_t i = 0; i < engine->model_count; i++) {
        if (engine->models[i].model_id == model_id && engine->models[i].is_trained) {
            for (uint32_t p = 0; p < forecast_periods && engine->prediction_count < MAX_PREDICTIONS; p++) {
                Prediction *pred = &engine->predictions[engine->prediction_count];
                memset(pred, 0, sizeof(Prediction));

                pred->prediction_id = next_prediction_id++;
                pred->model_id = model_id;
                pred->predicted_value = data[(p + data_count - 1) % data_count] * 1.01 * (p + 1);
                pred->confidence_interval_lower = pred->predicted_value * 0.95;
                pred->confidence_interval_upper = pred->predicted_value * 1.05;
                pred->prediction_timestamp = 0;
                pred->prediction_time = 0;

                engine->prediction_count++;
            }
            
            engine->analysis_operations++;
            return 0;
        }
    }

    return -1;
}

int analytics_detect_anomalies(AnalyticsEngine *engine, uint32_t metric_id,
                              const double *data, uint32_t data_count,
                              double threshold)
{
    if (!engine || metric_id == 0 || !data || data_count < 3)
        return -1;

    double sum = 0.0;
    for (uint32_t i = 0; i < data_count; i++) {
        sum += data[i];
    }
    double mean = sum / data_count;

    double variance = 0.0;
    for (uint32_t i = 0; i < data_count; i++) {
        variance += (data[i] - mean) * (data[i] - mean);
    }
    variance /= data_count;

    for (uint32_t i = 0; i < data_count; i++) {
        double deviation = data[i] - mean;
        
        if (deviation * deviation > variance * threshold * threshold && 
            engine->anomaly_count < MAX_ANOMALIES) {
            
            Anomaly *anomaly = &engine->anomalies[engine->anomaly_count];
            memset(anomaly, 0, sizeof(Anomaly));

            anomaly->anomaly_id = next_anomaly_id++;
            anomaly->metric_id = metric_id;
            anomaly->anomaly_type = ANOMALY_TYPE_STATISTICAL_OUTLIER;
            anomaly->observed_value = data[i];
            anomaly->expected_value = mean;
            anomaly->deviation = deviation;
            anomaly->anomaly_timestamp = 0;
            anomaly->consecutive_anomalies = 1;

            engine->anomaly_count++;
            engine->anomalies_detected++;
        }
    }

    engine->analysis_operations++;
    return engine->anomalies_detected;
}

int analytics_compute_correlations(AnalyticsEngine *engine, uint32_t metric_id_1,
                                  uint32_t metric_id_2, const double *data_1,
                                  const double *data_2, uint32_t data_count)
{
    if (!engine || metric_id_1 == 0 || metric_id_2 == 0 || !data_1 || !data_2 || data_count == 0)
        return -1;

    if (engine->correlation_count >= MAX_CORRELATIONS)
        return -1;

    double sum1 = 0.0, sum2 = 0.0;
    for (uint32_t i = 0; i < data_count; i++) {
        sum1 += data_1[i];
        sum2 += data_2[i];
    }
    double mean1 = sum1 / data_count;
    double mean2 = sum2 / data_count;

    double numerator = 0.0, denom1 = 0.0, denom2 = 0.0;
    for (uint32_t i = 0; i < data_count; i++) {
        numerator += (data_1[i] - mean1) * (data_2[i] - mean2);
        denom1 += (data_1[i] - mean1) * (data_1[i] - mean1);
        denom2 += (data_2[i] - mean2) * (data_2[i] - mean2);
    }

    double correlation = 0.0;
    if (denom1 > 0 && denom2 > 0) {
        correlation = numerator / (denom1 * denom2 * 0.5);
    }

    Correlation *corr = &engine->correlations[engine->correlation_count];
    memset(corr, 0, sizeof(Correlation));

    corr->correlation_id = next_correlation_id++;
    corr->metric_id_1 = metric_id_1;
    corr->metric_id_2 = metric_id_2;
    corr->correlation_coefficient = correlation;
    corr->is_significant = correlation > 0.7 ? 1 : 0;

    engine->correlation_count++;
    engine->analysis_operations++;

    return 0;
}

int analytics_detect_trends(AnalyticsEngine *engine, uint32_t metric_id,
                           const double *data, uint32_t data_count)
{
    if (!engine || metric_id == 0 || !data || data_count < 2)
        return -1;

    if (engine->trend_count >= MAX_TRENDS)
        return -1;

    double sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_x2 = 0.0;
    
    for (uint32_t i = 0; i < data_count; i++) {
        sum_x += i;
        sum_y += data[i];
        sum_xy += i * data[i];
        sum_x2 += i * i;
    }

    double slope = (data_count * sum_xy - sum_x * sum_y) / (data_count * sum_x2 - sum_x * sum_x);
    double intercept = (sum_y - slope * sum_x) / data_count;

    Trend *trend = &engine->trends[engine->trend_count];
    memset(trend, 0, sizeof(Trend));

    trend->trend_id = next_trend_id++;
    trend->metric_id = metric_id;
    trend->trend_slope = slope;
    trend->trend_intercept = intercept;
    trend->trend_start_time = 0;
    trend->trend_end_time = 0;

    engine->trend_count++;
    engine->analysis_operations++;

    return trend->trend_id;
}

int analytics_get_anomaly_count(AnalyticsEngine *engine, uint32_t metric_id)
{
    if (!engine)
        return 0;

    uint32_t count = 0;
    for (uint32_t i = 0; i < engine->anomaly_count; i++) {
        if (engine->anomalies[i].metric_id == metric_id) {
            count++;
        }
    }

    return count;
}

int analytics_get_prediction_count(AnalyticsEngine *engine, uint32_t model_id)
{
    if (!engine)
        return 0;

    uint32_t count = 0;
    for (uint32_t i = 0; i < engine->prediction_count; i++) {
        if (engine->predictions[i].model_id == model_id) {
            count++;
        }
    }

    return count;
}

int analytics_anomaly_clear(AnalyticsEngine *engine)
{
    if (!engine)
        return -1;

    engine->anomaly_count = 0;
    return 0;
}

int analytics_prediction_clear(AnalyticsEngine *engine)
{
    if (!engine)
        return -1;

    engine->prediction_count = 0;
    return 0;
}

int analytics_export_model(AnalyticsEngine *engine, uint32_t model_id,
                          uint8_t *buffer, uint32_t *buffer_length)
{
    if (!engine || model_id == 0 || !buffer || !buffer_length)
        return -1;

    for (uint32_t i = 0; i < engine->model_count; i++) {
        if (engine->models[i].model_id == model_id) {
            AnalyticsModel *model = &engine->models[i];
            uint32_t required_size = sizeof(AnalyticsModel) + model->coefficient_count * sizeof(double);
            
            if (required_size <= *buffer_length) {
                memcpy(buffer, model, sizeof(AnalyticsModel));
                memcpy(buffer + sizeof(AnalyticsModel), model->model_coefficients,
                       model->coefficient_count * sizeof(double));
                *buffer_length = required_size;
                return 0;
            }
            
            return -1;
        }
    }

    return -1;
}

int analytics_engine_get_metrics(AnalyticsEngine *engine, AnalyticsEngineMetrics *metrics)
{
    if (!engine || !metrics)
        return -1;

    uint32_t trained_models = 0;

    for (uint32_t i = 0; i < engine->model_count; i++) {
        if (engine->models[i].is_trained) {
            trained_models++;
        }
    }

    metrics->engine_id = engine->engine_id;
    metrics->total_models = engine->model_count;
    metrics->trained_models = trained_models;
    metrics->total_predictions = engine->prediction_count;
    metrics->total_anomalies = engine->anomaly_count;
    metrics->total_correlations = engine->correlation_count;
    metrics->total_trends = engine->trend_count;
    metrics->analysis_operations = engine->analysis_operations;
    metrics->average_model_accuracy = engine->overall_model_accuracy;

    return 0;
}
