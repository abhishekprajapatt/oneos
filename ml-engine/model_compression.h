#ifndef MODEL_COMPRESSION_H
#define MODEL_COMPRESSION_H

#include <stdint.h>
#include <stddef.h>

#define MAX_COMPRESSION_JOBS 128
#define MAX_COMPRESSED_MODELS 256
#define MAX_PRUNING_CONFIGS 64
#define MAX_QUANTIZATION_CONFIGS 32

typedef enum
{
    COMPRESSION_PRUNING,
    COMPRESSION_QUANTIZATION,
    COMPRESSION_DISTILLATION,
    COMPRESSION_SPARSITY,
    COMPRESSION_HUFFMAN,
    COMPRESSION_BROTLI
} CompressionType;

typedef enum
{
    PRUNING_MAGNITUDE,
    PRUNING_LOTTERY_TICKET,
    PRUNING_STRUCTURED,
    PRUNING_LAYER_WISE
} PruningType;

typedef enum
{
    QUANTIZATION_DYNAMIC,
    QUANTIZATION_STATIC,
    QUANTIZATION_QAT,
    QUANTIZATION_MIXED_PRECISION
} QuantizationType;

typedef struct
{
    uint32_t pruning_config_id;
    PruningType pruning_type;

    float pruning_ratio; // 0.0 - 1.0
    float threshold;
    uint32_t iterations;

    uint64_t original_size;
    uint64_t compressed_size;
    float compression_ratio;

    float accuracy_loss;
    float latency_improvement;
} PruningConfig;

typedef struct
{
    uint32_t quantization_config_id;
    QuantizationType quantization_type;

    uint8_t target_bits; // 8, 4, 2
    uint8_t weight_bits;
    uint8_t activation_bits;

    uint64_t original_size;
    uint64_t quantized_size;
    float compression_ratio;

    float accuracy_loss;
    float latency_improvement;
    float memory_savings;
} QuantizationConfig;

typedef struct
{
    uint32_t compression_job_id;
    uint32_t model_id;

    CompressionType compression_type;

    PruningConfig *pruning_config;
    QuantizationConfig *quantization_config;

    uint32_t job_status; // 0=pending, 1=running, 2=completed, 3=failed
    float progress_percent;

    uint64_t original_model_size;
    uint64_t compressed_model_size;
    float total_compression_ratio;

    uint32_t start_time;
    uint32_t end_time;
} CompressionJob;

typedef struct
{
    uint32_t model_id;
    char model_name[64];

    uint64_t original_size;
    uint64_t compressed_size;
    float compression_ratio;

    float original_accuracy;
    float compressed_accuracy;
    float accuracy_loss_percent;

    float original_latency_ms;
    float compressed_latency_ms;
    float speedup_factor;

    CompressionType compression_method;
    uint32_t compression_timestamp;
} CompressedModel;

typedef struct
{
    uint32_t compression_manager_id;

    uint32_t job_count;
    CompressionJob jobs[MAX_COMPRESSION_JOBS];

    uint32_t pruning_config_count;
    PruningConfig pruning_configs[MAX_PRUNING_CONFIGS];

    uint32_t quantization_config_count;
    QuantizationConfig quantization_configs[MAX_QUANTIZATION_CONFIGS];

    uint32_t compressed_model_count;
    CompressedModel compressed_models[MAX_COMPRESSED_MODELS];

    uint64_t total_original_size;
    uint64_t total_compressed_size;
    float average_compression_ratio;
} ModelCompressionManager;

// Manager initialization
ModelCompressionManager *compression_manager_init(void);
int compression_manager_destroy(ModelCompressionManager *manager);

// Compression job management
CompressionJob *compression_create_job(ModelCompressionManager *manager, uint32_t model_id, CompressionType type);
int compression_destroy_job(ModelCompressionManager *manager, uint32_t job_id);
int compression_start_job(ModelCompressionManager *manager, uint32_t job_id);

// Pruning operations
int compression_configure_pruning(ModelCompressionManager *manager, PruningType type, float pruning_ratio);
int compression_run_pruning(CompressionJob *job, PruningConfig *config);
int compression_apply_pruning(ModelCompressionManager *manager, uint32_t job_id);

// Quantization operations
int compression_configure_quantization(ModelCompressionManager *manager, QuantizationType type, uint8_t bits);
int compression_run_quantization(CompressionJob *job, QuantizationConfig *config);
int compression_apply_quantization(ModelCompressionManager *manager, uint32_t job_id);

// Knowledge distillation
int compression_configure_distillation(ModelCompressionManager *manager, uint32_t teacher_model_id, uint32_t student_model_id);
int compression_run_distillation(CompressionJob *job);
int compression_apply_distillation(ModelCompressionManager *manager, uint32_t job_id);

// Model archiving
int compression_save_compressed_model(ModelCompressionManager *manager, uint32_t model_id, const char *path);
int compression_load_compressed_model(ModelCompressionManager *manager, const char *path);

// Accuracy validation
int compression_validate_compressed_model(ModelCompressionManager *manager, uint32_t model_id);
int compression_benchmark_compressed_model(ModelCompressionManager *manager, uint32_t model_id);

// Monitoring
typedef struct
{
    uint32_t manager_id;
    uint32_t total_compressed_models;
    float average_compression_ratio;
    float average_speedup;
    float average_accuracy_loss;
    uint64_t total_size_savings;
} CompressionMetrics;

int compression_get_metrics(ModelCompressionManager *manager, CompressionMetrics *metrics);

// Optimization recommendations
int compression_recommend_compression_strategy(ModelCompressionManager *manager, uint32_t model_id);
int compression_get_pareto_frontier(ModelCompressionManager *manager);

#endif // MODEL_COMPRESSION_H
