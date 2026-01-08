#ifndef ML_INFERENCE_H
#define ML_INFERENCE_H

#include "ml_infrastructure.h"
#include <stdint.h>

#define MAX_INFERENCE_ENGINES 16
#define MAX_INFERENCE_CACHE 1024
#define MAX_QUANTIZATION_LEVELS 8

typedef enum
{
    PRECISION_FP32 = 0,
    PRECISION_FP16 = 1,
    PRECISION_INT8 = 2,
    PRECISION_INT16 = 3,
    PRECISION_MIXED = 4
} PrecisionType;

typedef enum
{
    INFERENCE_CPU = 0,
    INFERENCE_GPU = 1,
    INFERENCE_TPU = 2,
    INFERENCE_ASIC = 3,
    INFERENCE_FPGA = 4
} InferenceDevice;

typedef struct
{
    uint32_t engine_id;
    uint32_t model_id;
    InferenceDevice device;
    PrecisionType precision;
    uint32_t batch_size;
    float latency_ms;
    float throughput_ops;
    uint64_t total_inferences;
    bool is_optimized;
} InferenceEngine;

typedef struct
{
    uint32_t cache_entry_id;
    uint32_t model_id;
    uint32_t input_hash;
    float output[256];
    uint32_t output_size;
    uint64_t timestamp;
    uint32_t hit_count;
} InferenceCache;

typedef struct
{
    uint32_t quantization_id;
    float min_value;
    float max_value;
    int32_t bits;
    bool symmetric;
} QuantizationConfig;

typedef struct
{
    uint32_t model_id;
    InferenceEngine engines[MAX_INFERENCE_ENGINES];
    InferenceCache cache[MAX_INFERENCE_CACHE];
    QuantizationConfig quantization;
    uint32_t engine_count;
    uint32_t cache_count;
    uint32_t cache_hits;
    uint32_t cache_misses;
} MLInferenceManager;

MLInferenceManager *ml_inference_init(void);
void ml_inference_cleanup(MLInferenceManager *mgr);

uint32_t ml_create_inference_engine(MLInferenceManager *mgr, uint32_t model_id, InferenceDevice device);
int ml_optimize_model_for_inference(MLInferenceManager *mgr, uint32_t engine_id);
int ml_set_inference_precision(MLInferenceManager *mgr, uint32_t engine_id, PrecisionType precision);

int ml_quantize_model(MLInferenceManager *mgr, uint32_t model_id, float min_val, float max_val, int32_t bits);
int ml_prune_model(MLInferenceManager *mgr, uint32_t model_id, float threshold);
int ml_distill_model(MLInferenceManager *mgr, uint32_t teacher_model_id, uint32_t student_model_id, float temperature);

int ml_inference_with_cache(MLInferenceManager *mgr, uint32_t model_id, const float *input, uint32_t input_size, float *output, uint32_t output_size);
int ml_clear_inference_cache(MLInferenceManager *mgr);
int ml_get_cache_stats(MLInferenceManager *mgr, uint32_t *hits, uint32_t *misses, float *hit_rate);

int ml_get_inference_latency(MLInferenceManager *mgr, uint32_t engine_id, float *latency_ms);
int ml_get_inference_throughput(MLInferenceManager *mgr, uint32_t engine_id, float *ops_per_sec);
int ml_benchmark_inference(MLInferenceManager *mgr, uint32_t engine_id, uint32_t num_iterations);

#endif
