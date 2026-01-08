#ifndef ML_TRAINING_H
#define ML_TRAINING_H

#include "ml_infrastructure.h"
#include <stdint.h>

#define MAX_LEARNING_STRATEGIES 10
#define MAX_HYPERPARAMETERS 50
#define MAX_AUGMENTATION_TECHNIQUES 20

typedef enum
{
    AUGMENTATION_FLIP = 0,
    AUGMENTATION_ROTATION = 1,
    AUGMENTATION_ZOOM = 2,
    AUGMENTATION_SHIFT = 3,
    AUGMENTATION_BLUR = 4,
    AUGMENTATION_CONTRAST = 5,
    AUGMENTATION_BRIGHTNESS = 6,
    AUGMENTATION_NOISE = 8
} AugmentationType;

typedef enum
{
    OPTIMIZER_SGD = 0,
    OPTIMIZER_ADAM = 1,
    OPTIMIZER_RMSPROP = 2,
    OPTIMIZER_ADAGRAD = 3,
    OPTIMIZER_ADAMAX = 4
} OptimizerType;

typedef struct
{
    OptimizerType optimizer;
    float learning_rate;
    float momentum;
    float decay;
    float epsilon;
    float beta1;
    float beta2;
} TrainingConfig;

typedef struct
{
    uint32_t strategy_id;
    char strategy_name[64];
    float dropout_rate;
    uint32_t regularization_type;
    float regularization_strength;
    uint32_t early_stopping_patience;
    float validation_threshold;
} LearningStrategy;

typedef struct
{
    uint32_t technique_id;
    AugmentationType aug_type;
    float intensity;
    float probability;
    bool is_enabled;
} AugmentationTechnique;

typedef struct
{
    uint32_t ml_training_id;
    uint32_t model_id;
    TrainingConfig config;
    LearningStrategy strategies[MAX_LEARNING_STRATEGIES];
    AugmentationTechnique augmentations[MAX_AUGMENTATION_TECHNIQUES];
    uint32_t strategy_count;
    uint32_t augmentation_count;
} MLTraining;

MLTraining *ml_training_init(uint32_t model_id);
void ml_training_cleanup(MLTraining *ml_training);

int ml_set_optimizer(MLTraining *ml_training, OptimizerType optimizer, float learning_rate);
int ml_add_learning_strategy(MLTraining *ml_training, const char *name, float dropout, uint32_t regularization);
int ml_add_augmentation(MLTraining *ml_training, AugmentationType aug_type, float intensity, float probability);

int ml_apply_data_augmentation(MLTraining *ml_training, const float *original_data, uint32_t data_size, float *augmented_data);
int ml_apply_regularization(MLTraining *ml_training, const float *weights, uint32_t weight_count, float *regularized_loss);

int ml_validate_hyperparameters(MLTraining *ml_training);
int ml_get_training_config(MLTraining *ml_training, TrainingConfig *config);

#endif
