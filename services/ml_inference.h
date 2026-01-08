#ifndef ML_INFERENCE_H
#define ML_INFERENCE_H

#include <stdint.h>
#include <stddef.h>

#define MAX_MODELS 64
#define MAX_LAYERS 128
#define MAX_NEURONS_PER_LAYER 1024
#define MAX_INPUT_SIZE 8192
#define MAX_BATCH_SIZE 256

typedef enum
{
    MODEL_NEURAL_NETWORK = 0,
    MODEL_DECISION_TREE = 1,
    MODEL_RANDOM_FOREST = 2,
    MODEL_SVM = 3,
    MODEL_CNN = 4
} ModelType;

typedef enum
{
    ACTIVATION_LINEAR = 0,
    ACTIVATION_RELU = 1,
    ACTIVATION_SIGMOID = 2,
    ACTIVATION_TANH = 3,
    ACTIVATION_SOFTMAX = 4
} ActivationFunction;

typedef struct
{
    uint32_t layer_id;
    uint32_t input_size;
    uint32_t output_size;
    ActivationFunction activation;
    float *weights;
    float *biases;
    float *cache;
} NeuralLayer;

typedef struct
{
    uint32_t model_id;
    char name[64];
    ModelType type;
    uint32_t layer_count;
    NeuralLayer layers[MAX_LAYERS];
    uint32_t input_size;
    uint32_t output_size;
    uint64_t inference_count;
    uint64_t total_inference_time_ms;
    float avg_confidence;
    uint32_t version;
} MLModel;

typedef struct
{
    uint32_t batch_id;
    uint32_t sample_count;
    float *input_data;
    float *output_data;
    float *predictions;
    float *confidence_scores;
    uint32_t *predicted_classes;
    uint64_t processing_time_ms;
} InferenceBatch;

typedef struct
{
    uint32_t request_id;
    float *input_features;
    uint32_t input_size;
    float *output;
    uint32_t output_size;
    float confidence;
    uint32_t predicted_class;
    uint64_t inference_time_ms;
} InferenceRequest;

// Model management
int ml_create_model(const char *name, ModelType type, uint32_t input_size, uint32_t output_size);
int ml_load_model(uint32_t model_id, const char *path);
int ml_save_model(uint32_t model_id, const char *path);
int ml_delete_model(uint32_t model_id);

// Layer management
int ml_add_layer(uint32_t model_id, uint32_t input_size, uint32_t output_size, ActivationFunction activation);
int ml_set_layer_weights(uint32_t model_id, uint32_t layer_id, const float *weights, size_t weight_count);
int ml_set_layer_biases(uint32_t model_id, uint32_t layer_id, const float *biases, size_t bias_count);

// Inference operations
int ml_infer(uint32_t model_id, const float *input, uint32_t input_size, float *output, uint32_t output_size);
int ml_infer_batch(uint32_t model_id, const float *batch_data, uint32_t batch_size, uint32_t sample_size, float *outputs);
int ml_infer_with_confidence(uint32_t model_id, const float *input, uint32_t input_size, float *output, float *confidence);

// Training and optimization
int ml_train_model(uint32_t model_id, const float *training_data, uint32_t sample_count,
                   const float *labels, uint32_t epoch_count, float learning_rate);
int ml_validate_model(uint32_t model_id, const float *validation_data, uint32_t sample_count, const float *labels);
float ml_evaluate_accuracy(uint32_t model_id, const float *test_data, uint32_t sample_count, const float *labels);

// Model optimization
int ml_quantize_model(uint32_t model_id);
int ml_prune_model(uint32_t model_id, float threshold);
int ml_optimize_for_inference(uint32_t model_id);

// Model information and monitoring
MLModel *ml_get_model(uint32_t model_id);
uint32_t ml_get_model_count(void);
uint64_t ml_get_total_inference_time(uint32_t model_id);
float ml_get_avg_inference_time(uint32_t model_id);

#endif // ML_INFERENCE_H
