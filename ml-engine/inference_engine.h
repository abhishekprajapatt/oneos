#ifndef INFERENCE_ENGINE_H
#define INFERENCE_ENGINE_H

#include <stdint.h>
#include <stddef.h>

#define MAX_INFERENCE_SESSIONS 256
#define MAX_BATCH_SIZE 1024
#define MAX_INPUT_TENSORS 128
#define MAX_OUTPUT_TENSORS 64
#define MAX_INFERENCE_RESULTS 512

typedef enum
{
    COMPUTE_CPU,
    COMPUTE_GPU,
    COMPUTE_TPU,
    COMPUTE_ASIC,
    COMPUTE_FPGA,
    COMPUTE_HYBRID
} ComputeDevice;

typedef enum
{
    TENSOR_FLOAT32,
    TENSOR_FLOAT16,
    TENSOR_INT32,
    TENSOR_INT8,
    TENSOR_INT4,
    TENSOR_UINT8
} TensorDataType;

typedef struct
{
    uint32_t tensor_id;
    char tensor_name[64];

    uint32_t *shape;     // Tensor dimensions
    uint32_t shape_rank; // Number of dimensions

    TensorDataType dtype;
    uint64_t data_size;
    void *data;

    uint64_t created_timestamp;
} Tensor;

typedef struct
{
    uint32_t session_id;
    uint32_t model_id;

    ComputeDevice device;

    uint32_t input_tensor_count;
    Tensor *input_tensors[MAX_INPUT_TENSORS];

    uint32_t output_tensor_count;
    Tensor *output_tensors[MAX_OUTPUT_TENSORS];

    uint32_t batch_size;
    uint64_t total_inference_time_ms;
    uint64_t inference_count;

    float average_latency_ms;
    float throughput_inferences_per_sec;
} InferenceSession;

typedef struct
{
    uint32_t result_id;
    uint32_t session_id;
    uint32_t inference_id;

    float *predictions;
    uint32_t prediction_count;

    float confidence;
    uint32_t top_class;

    uint64_t inference_time_us;
    uint32_t timestamp;
} InferenceResult;

typedef struct
{
    uint32_t engine_id;

    uint32_t session_count;
    InferenceSession sessions[MAX_INFERENCE_SESSIONS];

    uint32_t result_count;
    InferenceResult results[MAX_INFERENCE_RESULTS];

    ComputeDevice preferred_device;

    uint64_t total_inferences_run;
    uint64_t total_inference_time_ms;

    float average_latency_ms;
    float peak_throughput_ips;

    // Resource usage
    uint64_t gpu_memory_used;
    uint64_t cpu_memory_used;
} InferenceEngine;

// Engine initialization
InferenceEngine *inference_engine_init(ComputeDevice device);
int inference_engine_destroy(InferenceEngine *engine);

// Session management
InferenceSession *inference_create_session(InferenceEngine *engine, uint32_t model_id);
int inference_destroy_session(InferenceEngine *engine, uint32_t session_id);
int inference_set_batch_size(InferenceSession *session, uint32_t batch_size);

// Tensor management
Tensor *inference_create_input_tensor(InferenceSession *session, const char *name,
                                      TensorDataType dtype, uint32_t *shape, uint32_t shape_rank);
Tensor *inference_create_output_tensor(InferenceSession *session, const char *name,
                                       TensorDataType dtype, uint32_t *shape, uint32_t shape_rank);
int inference_set_tensor_data(Tensor *tensor, const void *data, uint64_t data_size);
int inference_get_tensor_data(Tensor *tensor, void *data, uint64_t *data_size);

// Inference operations
InferenceResult *inference_run(InferenceEngine *engine, uint32_t session_id);
int inference_run_batch(InferenceEngine *engine, uint32_t session_id, uint32_t batch_count);
int inference_get_predictions(InferenceEngine *engine, uint32_t result_id, float **predictions, uint32_t *count);

// Optimization
int inference_optimize_for_latency(InferenceEngine *engine, uint32_t session_id);
int inference_optimize_for_throughput(InferenceEngine *engine, uint32_t session_id);
int inference_optimize_for_memory(InferenceEngine *engine, uint32_t session_id);

// Device management
int inference_set_device(InferenceSession *session, ComputeDevice device);
int inference_get_device_memory(InferenceEngine *engine, ComputeDevice device, uint64_t *total, uint64_t *available);
int inference_profile_device(InferenceEngine *engine, ComputeDevice device);

// Batch processing
int inference_configure_batch_processing(InferenceEngine *engine, uint32_t batch_size, uint32_t prefetch_count);
int inference_process_batch(InferenceEngine *engine, uint32_t session_id);

// Caching
int inference_enable_result_caching(InferenceEngine *engine);
int inference_disable_result_caching(InferenceEngine *engine);
InferenceResult *inference_get_cached_result(InferenceEngine *engine, uint32_t session_id);

// Warmup
int inference_warmup_model(InferenceEngine *engine, uint32_t session_id, uint32_t num_iterations);

// Monitoring
typedef struct
{
    uint32_t session_id;
    float average_latency_ms;
    float throughput_ips;
    uint64_t total_inferences;
    float gpu_utilization_percent;
    float memory_utilization_percent;
    float model_accuracy;
} InferenceMetrics;

int inference_get_metrics(InferenceSession *session, InferenceMetrics *metrics);
int inference_get_engine_metrics(InferenceEngine *engine);

// Model profiling
int inference_profile_model(InferenceEngine *engine, uint32_t session_id);
int inference_get_layer_statistics(InferenceEngine *engine, uint32_t session_id);

#endif // INFERENCE_ENGINE_H
