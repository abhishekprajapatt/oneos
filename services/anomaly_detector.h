#ifndef ANOMALY_DETECTOR_H
#define ANOMALY_DETECTOR_H

#include <stdint.h>
#include <stddef.h>

#define MAX_DETECTORS 32
#define MAX_BASELINE_POINTS 10000
#define MAX_ANOMALY_ALERTS 512
#define MAX_METRIC_NAME 64
#define MAX_THRESHOLD_RULES 128
#define BASELINE_WINDOW_SIZE 100

typedef enum
{
    STAT_ZSCORE = 1,
    STAT_IQR = 2,
    STAT_MOVING_AVG = 3,
    ISOLATION_FOREST = 4,
    LSTM_ANOMALY = 5
} DetectionMethod;

typedef enum
{
    ANOMALY_NONE = 0,
    ANOMALY_LOW = 1,
    ANOMALY_HIGH = 2,
    ANOMALY_PATTERN = 3,
    ANOMALY_OUTLIER = 4,
    ANOMALY_CRITICAL = 5
} AnomalySeverity;

typedef struct
{
    uint64_t timestamp;
    float value;
    uint32_t source_id;
} DataPoint;

typedef struct
{
    uint64_t point_ids[MAX_BASELINE_POINTS];
    float values[MAX_BASELINE_POINTS];
    uint64_t timestamps[MAX_BASELINE_POINTS];
    uint32_t point_count;
    float mean;
    float stddev;
    float min_value;
    float max_value;
} Baseline;

typedef struct
{
    uint32_t rule_id;
    char metric_name[MAX_METRIC_NAME];
    float lower_threshold;
    float upper_threshold;
    DetectionMethod method;
    float sensitivity;
    uint32_t window_size;
    uint32_t is_enabled;
} ThresholdRule;

typedef struct
{
    uint64_t alert_id;
    uint32_t rule_id;
    float value;
    float threshold;
    AnomalySeverity severity;
    uint64_t timestamp;
    char reason[256];
    uint32_t is_acknowledged;
} AnomalyAlert;

typedef struct
{
    uint32_t detector_id;
    char name[MAX_METRIC_NAME];
    DetectionMethod method;

    DataPoint data_points[MAX_BASELINE_POINTS];
    uint32_t data_point_count;

    Baseline baselines[8];
    uint32_t baseline_count;

    ThresholdRule rules[MAX_THRESHOLD_RULES];
    uint32_t rule_count;

    AnomalyAlert alerts[MAX_ANOMALY_ALERTS];
    uint32_t alert_count;

    float baseline_values[BASELINE_WINDOW_SIZE];
    uint32_t baseline_idx;

    float moving_avg;
    uint32_t total_anomalies;
    uint32_t critical_anomalies;
    uint64_t bytes_analyzed;

    float learning_rate;
    uint32_t is_learning;
} AnomalyDetector;

typedef struct
{
    float anomaly_scores[256];
    uint32_t anomaly_count;
    AnomalySeverity max_severity;
} DetectionResult;

int ad_create_detector(const char *name, DetectionMethod method);
int ad_delete_detector(uint32_t detector_id);
int ad_feed_data_point(uint32_t detector_id, float value, uint64_t timestamp);
int ad_feed_batch_data(uint32_t detector_id, float *values, uint64_t *timestamps, uint32_t count);

int ad_add_threshold_rule(uint32_t detector_id, const char *metric_name, float lower, float upper, float sensitivity);
int ad_remove_threshold_rule(uint32_t detector_id, uint32_t rule_id);
int ad_update_rule_sensitivity(uint32_t detector_id, uint32_t rule_id, float sensitivity);

int ad_learn_baseline(uint32_t detector_id, uint32_t window_size);
int ad_compute_baseline_stats(uint32_t detector_id, uint32_t baseline_idx);
int ad_update_baseline(uint32_t detector_id, uint32_t baseline_idx);

int ad_detect_anomalies(uint32_t detector_id, DetectionResult *result);
int ad_zscore_anomaly_detection(uint32_t detector_id, float value, float *z_score);
int ad_iqr_anomaly_detection(uint32_t detector_id, float value);
int ad_isolation_forest_score(uint32_t detector_id, float value);

int ad_generate_alert(uint32_t detector_id, uint32_t rule_id, float value, float threshold, AnomalySeverity severity);
int ad_get_alert(uint32_t detector_id, uint64_t alert_id, AnomalyAlert *alert);
int ad_acknowledge_alert(uint32_t detector_id, uint64_t alert_id);
int ad_get_critical_alerts(uint32_t detector_id, AnomalyAlert *alerts, uint32_t *count);

int ad_clear_old_alerts(uint32_t detector_id, uint64_t older_than_us);
int ad_clear_old_data(uint32_t detector_id, uint64_t older_than_us);

uint32_t ad_get_anomaly_count(uint32_t detector_id);
uint32_t ad_get_critical_count(uint32_t detector_id);
float ad_get_anomaly_rate(uint32_t detector_id);

AnomalyDetector *ad_get_detector_info(uint32_t detector_id);
int ad_export_alerts(uint32_t detector_id, const char *format);

#endif
