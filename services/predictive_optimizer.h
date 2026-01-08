#ifndef ONEOS_PREDICTIVE_OPTIMIZER_H
#define ONEOS_PREDICTIVE_OPTIMIZER_H

#include <stdint.h>
#include <time.h>

#define MAX_PREDICTIVE_OPTIMIZERS 16
#define MAX_PREDICTION_MODELS 64
#define MAX_HISTORICAL_DATA 4096
#define MAX_PREDICTIONS 2048
#define MAX_OPTIMIZATION_ACTIONS 512
#define MAX_ANOMALY_PATTERNS 256

typedef enum
{
    PREDICTION_MODEL_LINEAR_REGRESSION = 0,
    PREDICTION_MODEL_POLYNOMIAL = 1,
    PREDICTION_MODEL_EXPONENTIAL = 2,
    PREDICTION_MODEL_NEURAL_NETWORK = 3,
    PREDICTION_MODEL_LSTM = 4,
    PREDICTION_MODEL_ARIMA = 5
} PredictionModelType;

typedef enum
{
    METRIC_CPU_USAGE = 0,
    METRIC_MEMORY_USAGE = 1,
    METRIC_DISK_IO = 2,
    METRIC_NETWORK_BANDWIDTH = 3,
    METRIC_RESPONSE_TIME = 4,
    METRIC_ERROR_RATE = 5,
    METRIC_THROUGHPUT = 6,
    METRIC_POWER_CONSUMPTION = 8
} PredictionMetricType;

typedef enum
{
    PREDICTION_CONFIDENCE_LOW = 0,
    PREDICTION_CONFIDENCE_MEDIUM = 1,
    PREDICTION_CONFIDENCE_HIGH = 2,
    PREDICTION_CONFIDENCE_VERY_HIGH = 3
} PredictionConfidenceLevel;

typedef enum
{
    ANOMALY_TYPE_STATISTICAL = 0,
    ANOMALY_TYPE_ISOLATION_FOREST = 1,
    ANOMALY_TYPE_LSTM_AUTOENCODER = 2,
    ANOMALY_TYPE_ONE_CLASS_SVM = 3,
    ANOMALY_TYPE_GAUSSIAN_MIXTURE = 4
} AnomalyDetectionType;

typedef struct
{
    uint32_t model_id;
    char model_name[64];
    PredictionModelType model_type;
    PredictionMetricType metric_type;
    double accuracy;
    double mse;
    double rmse;
    uint32_t training_sample_count;
    time_t trained_at;
    time_t last_updated;
    uint8_t is_active;
} PredictionModel;

typedef struct
{
    uint64_t data_point_id;
    PredictionMetricType metric_type;
    time_t timestamp;
    double metric_value;
    uint32_t window_size_minutes;
    uint8_t is_training_data;
} HistoricalDataPoint;

typedef struct
{
    uint64_t prediction_id;
    uint32_t model_id;
    PredictionMetricType metric_type;
    time_t prediction_time;
    time_t predicted_for_time;
    double predicted_value;
    double lower_bound;
    double upper_bound;
    PredictionConfidenceLevel confidence;
    double confidence_score;
    uint8_t verified;
    double actual_value;
} Prediction;

typedef struct
{
    uint32_t pattern_id;
    char pattern_name[64];
    AnomalyDetectionType detection_type;
    double threshold;
    uint32_t pattern_vector[128];
    uint32_t pattern_vector_size;
    uint32_t occurrences;
    time_t first_detected;
    time_t last_detected;
    uint8_t is_active;
} AnomalyPattern;

typedef struct
{
    uint32_t optimizer_id;
    char name[64];

    uint32_t model_count;
    PredictionModel prediction_models[64];

    uint32_t historical_data_count;
    HistoricalDataPoint historical_data[4096];

    uint32_t prediction_count;
    Prediction predictions[2048];

    uint32_t pattern_count;
    AnomalyPattern anomaly_patterns[256];

    uint32_t correct_predictions;
    uint32_t total_predictions;
    double prediction_accuracy;

    uint64_t anomalies_detected;
    uint64_t false_positives;

    time_t created_at;
    time_t last_training;
    time_t last_prediction;
    uint8_t is_active;
} PredictiveOptimizer;

int po_create_optimizer(const char *name);
int po_delete_optimizer(uint32_t optimizer_id);

int po_add_prediction_model(uint32_t optimizer_id, const char *model_name,
                            PredictionModelType model_type, PredictionMetricType metric_type);
int po_train_prediction_model(uint32_t optimizer_id, uint32_t model_id);
int po_delete_prediction_model(uint32_t optimizer_id, uint32_t model_id);

int po_record_historical_data(uint32_t optimizer_id, PredictionMetricType metric_type,
                              double metric_value, uint8_t is_training_data);
int po_get_historical_data(uint32_t optimizer_id, HistoricalDataPoint *data, uint32_t *count);

uint64_t po_make_prediction(uint32_t optimizer_id, uint32_t model_id,
                            PredictionMetricType metric_type, uint32_t minutes_ahead);
int po_verify_prediction(uint32_t optimizer_id, uint64_t prediction_id, double actual_value);
int po_get_predictions(uint32_t optimizer_id, Prediction *predictions, uint32_t *count);

int po_detect_anomaly_pattern(uint32_t optimizer_id, const char *pattern_name,
                              AnomalyDetectionType detection_type, double threshold);
int po_check_for_anomalies(uint32_t optimizer_id, double current_metric_value,
                           PredictionMetricType metric_type, uint8_t *is_anomaly);
int po_delete_anomaly_pattern(uint32_t optimizer_id, uint32_t pattern_id);

uint32_t po_get_anomalies_detected(uint32_t optimizer_id);
uint32_t po_get_false_positives(uint32_t optimizer_id);
double po_get_prediction_accuracy(uint32_t optimizer_id);

int po_optimize_based_on_predictions(uint32_t optimizer_id);
int po_generate_optimization_recommendations(uint32_t optimizer_id, char *recommendations, uint32_t max_len);

PredictiveOptimizer *po_get_optimizer_info(uint32_t optimizer_id);

#endif
