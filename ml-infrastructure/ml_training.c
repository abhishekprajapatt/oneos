#include "ml_training.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

MLTraining *ml_training_init(uint32_t model_id)
{
    MLTraining *ml_training = (MLTraining *)malloc(sizeof(MLTraining));
    if (!ml_training)
        return NULL;

    memset(ml_training, 0, sizeof(MLTraining));
    ml_training->ml_training_id = 1;
    ml_training->model_id = model_id;

    ml_training->config.optimizer = OPTIMIZER_ADAM;
    ml_training->config.learning_rate = 0.001f;
    ml_training->config.momentum = 0.9f;
    ml_training->config.decay = 0.0f;
    ml_training->config.epsilon = 1e-8;
    ml_training->config.beta1 = 0.9f;
    ml_training->config.beta2 = 0.999f;

    ml_training->strategy_count = 0;
    ml_training->augmentation_count = 0;

    return ml_training;
}

void ml_training_cleanup(MLTraining *ml_training)
{
    if (!ml_training)
        return;
    memset(ml_training, 0, sizeof(MLTraining));
    free(ml_training);
}

int ml_set_optimizer(MLTraining *ml_training, OptimizerType optimizer, float learning_rate)
{
    if (!ml_training || learning_rate <= 0.0f)
        return -1;

    ml_training->config.optimizer = optimizer;
    ml_training->config.learning_rate = learning_rate;

    switch (optimizer)
    {
    case OPTIMIZER_SGD:
        ml_training->config.momentum = 0.9f;
        break;
    case OPTIMIZER_ADAM:
        ml_training->config.beta1 = 0.9f;
        ml_training->config.beta2 = 0.999f;
        ml_training->config.epsilon = 1e-8;
        break;
    case OPTIMIZER_RMSPROP:
        ml_training->config.decay = 0.99f;
        break;
    case OPTIMIZER_ADAGRAD:
        ml_training->config.epsilon = 1e-7;
        break;
    default:
        return -1;
    }

    return 0;
}

int ml_add_learning_strategy(MLTraining *ml_training, const char *name, float dropout, uint32_t regularization)
{
    if (!ml_training || !name || ml_training->strategy_count >= MAX_LEARNING_STRATEGIES)
        return -1;
    if (dropout < 0.0f || dropout > 1.0f)
        return -1;

    LearningStrategy *strategy = &ml_training->strategies[ml_training->strategy_count];
    strategy->strategy_id = ml_training->strategy_count + 1;
    strncpy(strategy->strategy_name, name, 63);
    strategy->dropout_rate = dropout;
    strategy->regularization_type = regularization;
    strategy->regularization_strength = 0.0001f;
    strategy->early_stopping_patience = 10;
    strategy->validation_threshold = 0.95f;

    ml_training->strategy_count++;
    return strategy->strategy_id;
}

int ml_add_augmentation(MLTraining *ml_training, AugmentationType aug_type, float intensity, float probability)
{
    if (!ml_training || ml_training->augmentation_count >= MAX_AUGMENTATION_TECHNIQUES)
        return -1;
    if (intensity < 0.0f || intensity > 1.0f || probability < 0.0f || probability > 1.0f)
        return -1;

    AugmentationTechnique *aug = &ml_training->augmentations[ml_training->augmentation_count];
    aug->technique_id = ml_training->augmentation_count + 1;
    aug->aug_type = aug_type;
    aug->intensity = intensity;
    aug->probability = probability;
    aug->is_enabled = true;

    ml_training->augmentation_count++;
    return aug->technique_id;
}

int ml_apply_data_augmentation(MLTraining *ml_training, const float *original_data, uint32_t data_size, float *augmented_data)
{
    if (!ml_training || !original_data || !augmented_data)
        return -1;

    memcpy(augmented_data, original_data, data_size * sizeof(float));

    for (uint32_t i = 0; i < ml_training->augmentation_count; i++)
    {
        AugmentationTechnique *aug = &ml_training->augmentations[i];
        if (!aug->is_enabled)
            continue;

        float rand_val = (float)(rand() % 100) / 100.0f;
        if (rand_val > aug->probability)
            continue;

        switch (aug->aug_type)
        {
        case AUGMENTATION_FLIP:
            for (uint32_t j = 0; j < data_size / 2; j++)
            {
                float temp = augmented_data[j];
                augmented_data[j] = augmented_data[data_size - 1 - j];
                augmented_data[data_size - 1 - j] = temp;
            }
            break;
        case AUGMENTATION_ROTATION:
            for (uint32_t j = 0; j < data_size; j++)
            {
                augmented_data[j] *= (1.0f + aug->intensity * 0.1f);
            }
            break;
        case AUGMENTATION_ZOOM:
            for (uint32_t j = 0; j < data_size; j++)
            {
                augmented_data[j] *= (1.0f + aug->intensity);
            }
            break;
        case AUGMENTATION_BRIGHTNESS:
            for (uint32_t j = 0; j < data_size; j++)
            {
                augmented_data[j] += aug->intensity * 0.2f;
            }
            break;
        default:
            break;
        }
    }

    return 0;
}

int ml_apply_regularization(MLTraining *ml_training, const float *weights, uint32_t weight_count, float *regularized_loss)
{
    if (!ml_training || !weights || !regularized_loss)
        return -1;

    *regularized_loss = 0.0f;

    for (uint32_t i = 0; i < ml_training->strategy_count; i++)
    {
        LearningStrategy *strategy = &ml_training->strategies[i];

        if (strategy->regularization_type == 1)
        {
            for (uint32_t j = 0; j < weight_count; j++)
            {
                *regularized_loss += strategy->regularization_strength * fabsf(weights[j]);
            }
        }
        else if (strategy->regularization_type == 2)
        {
            for (uint32_t j = 0; j < weight_count; j++)
            {
                *regularized_loss += strategy->regularization_strength * weights[j] * weights[j];
            }
        }
    }

    return 0;
}

int ml_validate_hyperparameters(MLTraining *ml_training)
{
    if (!ml_training)
        return -1;

    if (ml_training->config.learning_rate <= 0.0f || ml_training->config.learning_rate > 1.0f)
        return -1;
    if (ml_training->config.momentum < 0.0f || ml_training->config.momentum > 1.0f)
        return -1;
    if (ml_training->config.beta1 < 0.0f || ml_training->config.beta1 > 1.0f)
        return -1;
    if (ml_training->config.beta2 < 0.0f || ml_training->config.beta2 > 1.0f)
        return -1;

    for (uint32_t i = 0; i < ml_training->strategy_count; i++)
    {
        LearningStrategy *strategy = &ml_training->strategies[i];
        if (strategy->dropout_rate < 0.0f || strategy->dropout_rate > 1.0f)
            return -1;
        if (strategy->regularization_strength < 0.0f)
            return -1;
    }

    return 0;
}

int ml_get_training_config(MLTraining *ml_training, TrainingConfig *config)
{
    if (!ml_training || !config)
        return -1;

    memcpy(config, &ml_training->config, sizeof(TrainingConfig));
    return 0;
}
