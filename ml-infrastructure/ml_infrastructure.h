#ifndef ML_INFRASTRUCTURE_H
#define ML_INFRASTRUCTURE_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_MODELS 128
#define MAX_DATASETS 256
#define MAX_TRAINING_JOBS 512
#define MAX_MODEL_LAYERS 1024
#define MAX_INFERENCE_BATCH 4096

typedef enum
{
    MODEL_TYPE_NEURAL_NETWORK = 0,
    MODEL_TYPE_DECISION_TREE = 1,
    MODEL_TYPE_RANDOM_FOREST = 2,
    MODEL_TYPE_SVM = 3,
    MODEL_TYPE_LINEAR_REGRESSION = 4,
    MODEL_TYPE_GRADIENT_BOOSTING = 5,
    MODEL_TYPE_CNN = 6,
    MODEL_TYPE_RNN = 7,
    MODEL_TYPE_TRANSFORMER = 8,
    MODEL_TYPE_ENSEMBLE = 9
} ModelType;

typedef enum
{
    FRAMEWORK_TENSORFLOW = 0,
    FRAMEWORK_PYTORCH = 1,
    FRAMEWORK_SKLEARN = 2,
    FRAMEWORK_XGBOOST = 3,
    FRAMEWORK_CAFFE = 4,
    FRAMEWORK_MXNet = 5
} MLFramework;

typedef struct
{
    uint32_t model_id;
    char model_name[128];
    ModelType type;
    MLFramework framework;
    uint32_t version;
    uint64_t parameter_count;
    float accuracy;
    float precision;
    float recall;
    float f1_score;
    bool is_trained;
    uint64_t training_time_ms;
} MLModel;

typedef struct
{
    uint32_t dataset_id;
    char dataset_name[128];
    uint64_t sample_count;
    uint32_t feature_count;
    uint32_t label_count;
    float train_split;
    float test_split;
    float validation_split;
    bool is_preprocessed;
} Dataset;

typedef struct
{
    uint32_t job_id;
    uint32_t model_id;
    uint32_t dataset_id;
    ModelType model_type;
    uint32_t epochs;
    uint32_t batch_size;
    float learning_rate;
    uint32_t status;
    float current_loss;
    float current_accuracy;
    uint64_t start_time;
    uint64_t end_time;
} TrainingJob;

typedef struct
{
    uint32_t model_count;
    uint32_t dataset_count;
    uint32_t training_job_count;
    MLModel models[MAX_MODELS];
    Dataset datasets[MAX_DATASETS];
    TrainingJob training_jobs[MAX_TRAINING_JOBS];
} MLInfrastructure;

MLInfrastructure *ml_init(void);
void ml_cleanup(MLInfrastructure *ml);

uint32_t ml_create_model(MLInfrastructure *ml, const char *name, ModelType type, MLFramework framework);
int ml_load_model(MLInfrastructure *ml, uint32_t model_id, const char *path);
int ml_save_model(MLInfrastructure *ml, uint32_t model_id, const char *path);
int ml_delete_model(MLInfrastructure *ml, uint32_t model_id);

uint32_t ml_create_dataset(MLInfrastructure *ml, const char *name, uint64_t sample_count, uint32_t feature_count);
int ml_load_dataset(MLInfrastructure *ml, uint32_t dataset_id, const char *path);
int ml_preprocess_dataset(MLInfrastructure *ml, uint32_t dataset_id);
int ml_split_dataset(MLInfrastructure *ml, uint32_t dataset_id, float train, float test, float valid);

uint32_t ml_create_training_job(MLInfrastructure *ml, uint32_t model_id, uint32_t dataset_id, uint32_t epochs);
int ml_start_training(MLInfrastructure *ml, uint32_t job_id);
int ml_stop_training(MLInfrastructure *ml, uint32_t job_id);
int ml_get_training_progress(MLInfrastructure *ml, uint32_t job_id, float *loss, float *accuracy);

int ml_inference(MLInfrastructure *ml, uint32_t model_id, const float *input, uint32_t input_size, float *output, uint32_t output_size);
int ml_batch_inference(MLInfrastructure *ml, uint32_t model_id, const float *inputs, uint32_t batch_size, uint32_t input_size, float *outputs);

int ml_evaluate_model(MLInfrastructure *ml, uint32_t model_id, uint32_t dataset_id, float *accuracy, float *precision, float *recall);
int ml_cross_validate(MLInfrastructure *ml, uint32_t model_id, uint32_t dataset_id, uint32_t k_folds);

#endif
