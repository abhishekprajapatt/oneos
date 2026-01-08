#include "anomaly_detector.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

static AnomalyDetector g_detectors[MAX_DETECTORS] = {0};
static uint32_t g_detector_count = 0;
static uint64_t g_alert_id_counter = 1;

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

static float compute_percentile(float *values, uint32_t count, float percentile)
{
    if (count == 0)
        return 0.0f;
    uint32_t idx = (uint32_t)(count * percentile / 100.0f);
    if (idx >= count)
        idx = count - 1;
    return values[idx];
}

int ad_create_detector(const char *name, DetectionMethod method)
{
    if (!name || g_detector_count >= MAX_DETECTORS)
    {
        return -1;
    }

    AnomalyDetector *detector = &g_detectors[g_detector_count];
    detector->detector_id = g_detector_count + 1;
    strncpy(detector->name, name, MAX_METRIC_NAME - 1);
    detector->method = method;
    detector->data_point_count = 0;
    detector->baseline_count = 0;
    detector->rule_count = 0;
    detector->alert_count = 0;
    detector->baseline_idx = 0;
    detector->moving_avg = 0.0f;
    detector->total_anomalies = 0;
    detector->critical_anomalies = 0;
    detector->bytes_analyzed = 0;
    detector->learning_rate = 0.1f;
    detector->is_learning = 1;

    g_detector_count++;
    return detector->detector_id;
}

int ad_delete_detector(uint32_t detector_id)
{
    if (detector_id == 0 || detector_id > g_detector_count)
    {
        return -1;
    }

    for (uint32_t i = detector_id - 1; i < g_detector_count - 1; i++)
    {
        g_detectors[i] = g_detectors[i + 1];
    }
    g_detector_count--;

    return 0;
}

int ad_feed_data_point(uint32_t detector_id, float value, uint64_t timestamp)
{
    if (detector_id == 0 || detector_id > g_detector_count)
    {
        return -1;
    }

    AnomalyDetector *detector = &g_detectors[detector_id - 1];
    if (detector->data_point_count >= MAX_BASELINE_POINTS)
    {
        return -1;
    }

    DataPoint *point = &detector->data_points[detector->data_point_count];
    point->timestamp = timestamp;
    point->value = value;
    point->source_id = detector_id;

    detector->baseline_values[detector->baseline_idx] = value;
    detector->baseline_idx = (detector->baseline_idx + 1) % BASELINE_WINDOW_SIZE;

    detector->moving_avg = detector->moving_avg * (1.0f - detector->learning_rate) +
                           value * detector->learning_rate;

    detector->data_point_count++;
    detector->bytes_analyzed += sizeof(DataPoint);

    return 0;
}

int ad_feed_batch_data(uint32_t detector_id, float *values, uint64_t *timestamps, uint32_t count)
{
    if (detector_id == 0 || detector_id > g_detector_count || !values || !timestamps)
    {
        return -1;
    }

    for (uint32_t i = 0; i < count; i++)
    {
        if (ad_feed_data_point(detector_id, values[i], timestamps[i]) != 0)
        {
            return -1;
        }
    }

    return 0;
}

int ad_add_threshold_rule(uint32_t detector_id, const char *metric_name, float lower, float upper, float sensitivity)
{
    if (detector_id == 0 || detector_id > g_detector_count || !metric_name)
    {
        return -1;
    }

    AnomalyDetector *detector = &g_detectors[detector_id - 1];
    if (detector->rule_count >= MAX_THRESHOLD_RULES)
    {
        return -1;
    }

    ThresholdRule *rule = &detector->rules[detector->rule_count];
    rule->rule_id = detector->rule_count + 1;
    strncpy(rule->metric_name, metric_name, MAX_METRIC_NAME - 1);
    rule->lower_threshold = lower;
    rule->upper_threshold = upper;
    rule->method = detector->method;
    rule->sensitivity = sensitivity;
    rule->window_size = BASELINE_WINDOW_SIZE;
    rule->is_enabled = 1;

    detector->rule_count++;

    return rule->rule_id;
}

int ad_remove_threshold_rule(uint32_t detector_id, uint32_t rule_id)
{
    if (detector_id == 0 || detector_id > g_detector_count)
    {
        return -1;
    }

    AnomalyDetector *detector = &g_detectors[detector_id - 1];

    for (uint32_t i = 0; i < detector->rule_count; i++)
    {
        if (detector->rules[i].rule_id == rule_id)
        {
            for (uint32_t j = i; j < detector->rule_count - 1; j++)
            {
                detector->rules[j] = detector->rules[j + 1];
            }
            detector->rule_count--;
            return 0;
        }
    }

    return -1;
}

int ad_update_rule_sensitivity(uint32_t detector_id, uint32_t rule_id, float sensitivity)
{
    if (detector_id == 0 || detector_id > g_detector_count)
    {
        return -1;
    }

    AnomalyDetector *detector = &g_detectors[detector_id - 1];

    for (uint32_t i = 0; i < detector->rule_count; i++)
    {
        if (detector->rules[i].rule_id == rule_id)
        {
            detector->rules[i].sensitivity = sensitivity;
            return 0;
        }
    }

    return -1;
}

int ad_learn_baseline(uint32_t detector_id, uint32_t window_size)
{
    if (detector_id == 0 || detector_id > g_detector_count)
    {
        return -1;
    }

    AnomalyDetector *detector = &g_detectors[detector_id - 1];
    if (detector->data_point_count < window_size)
    {
        return -1;
    }

    if (detector->baseline_count >= 8)
    {
        return -1;
    }

    Baseline *baseline = &detector->baselines[detector->baseline_count];
    uint32_t count = (window_size > detector->data_point_count) ? detector->data_point_count : window_size;

    baseline->point_count = count;
    baseline->min_value = detector->data_points[0].value;
    baseline->max_value = detector->data_points[0].value;

    for (uint32_t i = 0; i < count; i++)
    {
        baseline->point_ids[i] = detector->data_points[i].timestamp;
        baseline->values[i] = detector->data_points[i].value;
        baseline->timestamps[i] = detector->data_points[i].timestamp;

        if (baseline->values[i] < baseline->min_value)
        {
            baseline->min_value = baseline->values[i];
        }
        if (baseline->values[i] > baseline->max_value)
        {
            baseline->max_value = baseline->values[i];
        }
    }

    baseline->mean = compute_mean(baseline->values, count);
    baseline->stddev = compute_stddev(baseline->values, count, baseline->mean);

    detector->baseline_count++;
    detector->is_learning = 1;

    return 0;
}

int ad_compute_baseline_stats(uint32_t detector_id, uint32_t baseline_idx)
{
    if (detector_id == 0 || detector_id > g_detector_count)
    {
        return -1;
    }

    AnomalyDetector *detector = &g_detectors[detector_id - 1];
    if (baseline_idx >= detector->baseline_count)
    {
        return -1;
    }

    Baseline *baseline = &detector->baselines[baseline_idx];
    baseline->mean = compute_mean(baseline->values, baseline->point_count);
    baseline->stddev = compute_stddev(baseline->values, baseline->point_count, baseline->mean);

    return 0;
}

int ad_update_baseline(uint32_t detector_id, uint32_t baseline_idx)
{
    return ad_compute_baseline_stats(detector_id, baseline_idx);
}

int ad_zscore_anomaly_detection(uint32_t detector_id, float value, float *z_score)
{
    if (detector_id == 0 || detector_id > g_detector_count || !z_score)
    {
        return -1;
    }

    AnomalyDetector *detector = &g_detectors[detector_id - 1];
    if (detector->baseline_count == 0)
    {
        return -1;
    }

    Baseline *baseline = &detector->baselines[0];
    if (baseline->stddev == 0.0f)
    {
        *z_score = 0.0f;
        return 0;
    }

    *z_score = (value - baseline->mean) / baseline->stddev;
    return 0;
}

int ad_iqr_anomaly_detection(uint32_t detector_id, float value)
{
    if (detector_id == 0 || detector_id > g_detector_count)
    {
        return -1;
    }

    AnomalyDetector *detector = &g_detectors[detector_id - 1];
    if (detector->data_point_count < 4)
    {
        return 0;
    }

    float *values = (float *)malloc(detector->data_point_count * sizeof(float));
    if (!values)
        return -1;

    for (uint32_t i = 0; i < detector->data_point_count; i++)
    {
        values[i] = detector->data_points[i].value;
    }

    float q1 = compute_percentile(values, detector->data_point_count, 25.0f);
    float q3 = compute_percentile(values, detector->data_point_count, 75.0f);
    float iqr = q3 - q1;
    float lower_bound = q1 - 1.5f * iqr;
    float upper_bound = q3 + 1.5f * iqr;

    free(values);

    return (value < lower_bound || value > upper_bound) ? 1 : 0;
}

int ad_isolation_forest_score(uint32_t detector_id, float value)
{
    if (detector_id == 0 || detector_id > g_detector_count)
    {
        return -1;
    }

    AnomalyDetector *detector = &g_detectors[detector_id - 1];
    if (detector->baseline_count == 0 || detector->baselines[0].point_count == 0)
    {
        return 0;
    }

    Baseline *baseline = &detector->baselines[0];
    float min_val = baseline->min_value;
    float max_val = baseline->max_value;
    float range = max_val - min_val;

    if (range == 0.0f)
        return 0;

    float normalized = (value - min_val) / range;

    if (normalized < 0.1f || normalized > 0.9f)
    {
        return 1;
    }

    return 0;
}

int ad_detect_anomalies(uint32_t detector_id, DetectionResult *result)
{
    if (detector_id == 0 || detector_id > g_detector_count || !result)
    {
        return -1;
    }

    AnomalyDetector *detector = &g_detectors[detector_id - 1];
    if (detector->data_point_count == 0)
    {
        return -1;
    }

    result->anomaly_count = 0;
    result->max_severity = ANOMALY_NONE;

    uint32_t sample_size = (detector->data_point_count < 256) ? detector->data_point_count : 256;

    for (uint32_t i = detector->data_point_count - sample_size; i < detector->data_point_count; i++)
    {
        float value = detector->data_points[i].value;
        float z_score = 0.0f;
        int is_anomaly = 0;
        AnomalySeverity severity = ANOMALY_NONE;

        if (detector->method == STAT_ZSCORE)
        {
            ad_zscore_anomaly_detection(detector_id, value, &z_score);
            if (fabsf(z_score) > 3.0f)
            {
                is_anomaly = 1;
                severity = ANOMALY_CRITICAL;
            }
            else if (fabsf(z_score) > 2.0f)
            {
                is_anomaly = 1;
                severity = ANOMALY_HIGH;
            }
        }
        else if (detector->method == STAT_IQR)
        {
            is_anomaly = ad_iqr_anomaly_detection(detector_id, value);
            severity = is_anomaly ? ANOMALY_HIGH : ANOMALY_NONE;
        }
        else if (detector->method == ISOLATION_FOREST)
        {
            is_anomaly = ad_isolation_forest_score(detector_id, value);
            severity = is_anomaly ? ANOMALY_OUTLIER : ANOMALY_NONE;
        }

        if (is_anomaly && result->anomaly_count < 256)
        {
            result->anomaly_scores[result->anomaly_count++] = fabsf(z_score);
            if (severity > result->max_severity)
            {
                result->max_severity = severity;
            }
            detector->total_anomalies++;
            if (severity == ANOMALY_CRITICAL)
            {
                detector->critical_anomalies++;
            }
        }
    }

    return 0;
}

int ad_generate_alert(uint32_t detector_id, uint32_t rule_id, float value, float threshold, AnomalySeverity severity)
{
    if (detector_id == 0 || detector_id > g_detector_count)
    {
        return -1;
    }

    AnomalyDetector *detector = &g_detectors[detector_id - 1];
    if (detector->alert_count >= MAX_ANOMALY_ALERTS)
    {
        return -1;
    }

    AnomalyAlert *alert = &detector->alerts[detector->alert_count];
    alert->alert_id = g_alert_id_counter++;
    alert->rule_id = rule_id;
    alert->value = value;
    alert->threshold = threshold;
    alert->severity = severity;
    alert->timestamp = 0;
    alert->is_acknowledged = 0;
    snprintf(alert->reason, 256, "Value %.2f exceeds threshold %.2f", value, threshold);

    detector->alert_count++;

    return alert->alert_id;
}

int ad_get_alert(uint32_t detector_id, uint64_t alert_id, AnomalyAlert *alert)
{
    if (detector_id == 0 || detector_id > g_detector_count || !alert)
    {
        return -1;
    }

    AnomalyDetector *detector = &g_detectors[detector_id - 1];

    for (uint32_t i = 0; i < detector->alert_count; i++)
    {
        if (detector->alerts[i].alert_id == alert_id)
        {
            *alert = detector->alerts[i];
            return 0;
        }
    }

    return -1;
}

int ad_acknowledge_alert(uint32_t detector_id, uint64_t alert_id)
{
    if (detector_id == 0 || detector_id > g_detector_count)
    {
        return -1;
    }

    AnomalyDetector *detector = &g_detectors[detector_id - 1];

    for (uint32_t i = 0; i < detector->alert_count; i++)
    {
        if (detector->alerts[i].alert_id == alert_id)
        {
            detector->alerts[i].is_acknowledged = 1;
            return 0;
        }
    }

    return -1;
}

int ad_get_critical_alerts(uint32_t detector_id, AnomalyAlert *alerts, uint32_t *count)
{
    if (detector_id == 0 || detector_id > g_detector_count || !alerts || !count)
    {
        return -1;
    }

    AnomalyDetector *detector = &g_detectors[detector_id - 1];
    uint32_t alert_count = 0;

    for (uint32_t i = 0; i < detector->alert_count && alert_count < *count; i++)
    {
        if (detector->alerts[i].severity == ANOMALY_CRITICAL)
        {
            alerts[alert_count++] = detector->alerts[i];
        }
    }

    *count = alert_count;
    return 0;
}

int ad_clear_old_alerts(uint32_t detector_id, uint64_t older_than_us)
{
    if (detector_id == 0 || detector_id > g_detector_count)
    {
        return -1;
    }

    return 0;
}

int ad_clear_old_data(uint32_t detector_id, uint64_t older_than_us)
{
    if (detector_id == 0 || detector_id > g_detector_count)
    {
        return -1;
    }

    return 0;
}

uint32_t ad_get_anomaly_count(uint32_t detector_id)
{
    if (detector_id == 0 || detector_id > g_detector_count)
    {
        return 0;
    }

    return g_detectors[detector_id - 1].total_anomalies;
}

uint32_t ad_get_critical_count(uint32_t detector_id)
{
    if (detector_id == 0 || detector_id > g_detector_count)
    {
        return 0;
    }

    return g_detectors[detector_id - 1].critical_anomalies;
}

float ad_get_anomaly_rate(uint32_t detector_id)
{
    if (detector_id == 0 || detector_id > g_detector_count)
    {
        return 0.0f;
    }

    AnomalyDetector *detector = &g_detectors[detector_id - 1];
    if (detector->data_point_count == 0)
    {
        return 0.0f;
    }

    return (float)detector->total_anomalies / detector->data_point_count;
}

AnomalyDetector *ad_get_detector_info(uint32_t detector_id)
{
    if (detector_id == 0 || detector_id > g_detector_count)
    {
        return NULL;
    }

    return &g_detectors[detector_id - 1];
}

int ad_export_alerts(uint32_t detector_id, const char *format)
{
    if (detector_id == 0 || detector_id > g_detector_count || !format)
    {
        return -1;
    }

    return 0;
}
