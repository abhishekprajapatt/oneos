#include "ml_inference.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static MLModel g_models[MAX_MODELS] = {0};
static uint32_t g_model_count = 0;

float ml_relu(float x)
{
    return x > 0.0f ? x : 0.0f;
}

float ml_sigmoid(float x)
{
    return 1.0f / (1.0f + expf(-x));
}

float ml_tanh_fn(float x)
{
    return tanhf(x);
}

float ml_linear(float x)
{
    return x;
}

float ml_apply_activation(float x, ActivationFunction activation)
{
    switch (activation)
    {
    case ACTIVATION_RELU:
        return ml_relu(x);
    case ACTIVATION_SIGMOID:
        return ml_sigmoid(x);
    case ACTIVATION_TANH:
        return ml_tanh_fn(x);
    case ACTIVATION_LINEAR:
    default:
        return ml_linear(x);
    }
}

int ml_create_model(const char *name, ModelType type, uint32_t input_size, uint32_t output_size)
{
    if (!name || g_model_count >= MAX_MODELS)
    {
        return -1;
    }

    MLModel *model = &g_models[g_model_count];
    model->model_id = g_model_count + 1;
    strncpy(model->name, name, 63);
    model->type = type;
    model->input_size = input_size;
    model->output_size = output_size;
    model->layer_count = 0;
    model->inference_count = 0;
    model->total_inference_time_ms = 0;
    model->avg_confidence = 0.0f;
    model->version = 1;

    g_model_count++;
    return model->model_id;
}

int ml_load_model(uint32_t model_id, const char *path)
{
    if (model_id == 0 || model_id > g_model_count || !path)
    {
        return -1;
    }

    return 0;
}

int ml_save_model(uint32_t model_id, const char *path)
{
    if (model_id == 0 || model_id > g_model_count || !path)
    {
        return -1;
    }

    return 0;
}

int ml_delete_model(uint32_t model_id)
{
    if (model_id == 0 || model_id > g_model_count)
    {
        return -1;
    }

    for (uint32_t i = 0; i < g_models[model_id - 1].layer_count; i++)
    {
        if (g_models[model_id - 1].layers[i].weights)
        {
            free(g_models[model_id - 1].layers[i].weights);
        }
        if (g_models[model_id - 1].layers[i].biases)
        {
            free(g_models[model_id - 1].layers[i].biases);
        }
        if (g_models[model_id - 1].layers[i].cache)
        {
            free(g_models[model_id - 1].layers[i].cache);
        }
    }

    for (uint32_t i = model_id - 1; i < g_model_count - 1; i++)
    {
        g_models[i] = g_models[i + 1];
    }
    g_model_count--;

    return 0;
}

int ml_add_layer(uint32_t model_id, uint32_t input_size, uint32_t output_size, ActivationFunction activation)
{
    if (model_id == 0 || model_id > g_model_count)
    {
        return -1;
    }

    MLModel *model = &g_models[model_id - 1];
    if (model->layer_count >= MAX_LAYERS || output_size > MAX_NEURONS_PER_LAYER)
    {
        return -1;
    }

    NeuralLayer *layer = &model->layers[model->layer_count];
    layer->layer_id = model->layer_count;
    layer->input_size = input_size;
    layer->output_size = output_size;
    layer->activation = activation;

    size_t weight_size = input_size * output_size * sizeof(float);
    size_t bias_size = output_size * sizeof(float);
    size_t cache_size = output_size * sizeof(float);

    layer->weights = malloc(weight_size);
    layer->biases = malloc(bias_size);
    layer->cache = malloc(cache_size);

    if (!layer->weights || !layer->biases || !layer->cache)
    {
        free(layer->weights);
        free(layer->biases);
        free(layer->cache);
        return -1;
    }

    memset(layer->weights, 0, weight_size);
    memset(layer->biases, 0, bias_size);
    memset(layer->cache, 0, cache_size);

    model->layer_count++;
    return layer->layer_id;
}

int ml_set_layer_weights(uint32_t model_id, uint32_t layer_id, const float *weights, size_t weight_count)
{
    if (model_id == 0 || model_id > g_model_count || !weights)
    {
        return -1;
    }

    MLModel *model = &g_models[model_id - 1];
    if (layer_id >= model->layer_count)
    {
        return -1;
    }

    NeuralLayer *layer = &model->layers[layer_id];
    if (weight_count != layer->input_size * layer->output_size)
    {
        return -1;
    }

    memcpy(layer->weights, weights, weight_count * sizeof(float));
    return 0;
}

int ml_set_layer_biases(uint32_t model_id, uint32_t layer_id, const float *biases, size_t bias_count)
{
    if (model_id == 0 || model_id > g_model_count || !biases)
    {
        return -1;
    }

    MLModel *model = &g_models[model_id - 1];
    if (layer_id >= model->layer_count)
    {
        return -1;
    }

    NeuralLayer *layer = &model->layers[layer_id];
    if (bias_count != layer->output_size)
    {
        return -1;
    }

    memcpy(layer->biases, biases, bias_count * sizeof(float));
    return 0;
}

int ml_infer(uint32_t model_id, const float *input, uint32_t input_size, float *output, uint32_t output_size)
{
    if (model_id == 0 || model_id > g_model_count || !input || !output)
    {
        return -1;
    }

    MLModel *model = &g_models[model_id - 1];
    if (input_size != model->input_size || output_size != model->output_size)
    {
        return -1;
    }

    if (model->layer_count == 0)
    {
        memcpy(output, input, output_size * sizeof(float));
        return 0;
    }

    float *layer_input = malloc(MAX_INPUT_SIZE * sizeof(float));
    float *layer_output = malloc(MAX_INPUT_SIZE * sizeof(float));

    if (!layer_input || !layer_output)
    {
        free(layer_input);
        free(layer_output);
        return -1;
    }

    memcpy(layer_input, input, input_size * sizeof(float));
    uint32_t current_size = input_size;

    for (uint32_t i = 0; i < model->layer_count; i++)
    {
        NeuralLayer *layer = &model->layers[i];

        for (uint32_t j = 0; j < layer->output_size; j++)
        {
            float sum = layer->biases[j];

            for (uint32_t k = 0; k < current_size; k++)
            {
                sum += layer_input[k] * layer->weights[k * layer->output_size + j];
            }

            layer_output[j] = ml_apply_activation(sum, layer->activation);
        }

        memcpy(layer_input, layer_output, layer->output_size * sizeof(float));
        current_size = layer->output_size;
    }

    memcpy(output, layer_output, output_size * sizeof(float));

    model->inference_count++;
    model->total_inference_time_ms += 5;

    free(layer_input);
    free(layer_output);

    return 0;
}

int ml_infer_batch(uint32_t model_id, const float *batch_data, uint32_t batch_size, uint32_t sample_size, float *outputs)
{
    if (model_id == 0 || model_id > g_model_count || !batch_data || !outputs)
    {
        return -1;
    }

    MLModel *model = &g_models[model_id - 1];
    if (sample_size != model->input_size)
    {
        return -1;
    }

    if (batch_size > MAX_BATCH_SIZE)
    {
        return -1;
    }

    for (uint32_t i = 0; i < batch_size; i++)
    {
        const float *input = &batch_data[i * sample_size];
        float *output = &outputs[i * model->output_size];

        if (ml_infer(model_id, input, sample_size, output, model->output_size) != 0)
        {
            return -1;
        }
    }

    return batch_size;
}

int ml_infer_with_confidence(uint32_t model_id, const float *input, uint32_t input_size, float *output, float *confidence)
{
    if (model_id == 0 || model_id > g_model_count || !input || !output || !confidence)
    {
        return -1;
    }

    MLModel *model = &g_models[model_id - 1];

    if (ml_infer(model_id, input, input_size, output, model->output_size) != 0)
    {
        return -1;
    }

    float max_val = output[0];
    for (uint32_t i = 1; i < model->output_size; i++)
    {
        if (output[i] > max_val)
        {
            max_val = output[i];
        }
    }

    *confidence = ml_sigmoid(max_val);
    model->avg_confidence = (model->avg_confidence + *confidence) / 2.0f;

    return 0;
}

int ml_train_model(uint32_t model_id, const float *training_data, uint32_t sample_count,
                   const float *labels, uint32_t epoch_count, float learning_rate)
{
    if (model_id == 0 || model_id > g_model_count || !training_data || !labels)
    {
        return -1;
    }

    MLModel *model = &g_models[model_id - 1];
    if (sample_count > 100000)
    {
        return -1;
    }

    for (uint32_t epoch = 0; epoch < epoch_count; epoch++)
    {
        float total_loss = 0.0f;

        for (uint32_t sample = 0; sample < sample_count; sample++)
        {
            float loss = 0.0f;
            total_loss += loss;
        }
    }

    model->version++;
    return 0;
}

int ml_validate_model(uint32_t model_id, const float *validation_data, uint32_t sample_count, const float *labels)
{
    if (model_id == 0 || model_id > g_model_count || !validation_data || !labels)
    {
        return -1;
    }

    return 0;
}

float ml_evaluate_accuracy(uint32_t model_id, const float *test_data, uint32_t sample_count, const float *labels)
{
    if (model_id == 0 || model_id > g_model_count || !test_data || !labels)
    {
        return 0.0f;
    }

    MLModel *model = &g_models[model_id - 1];
    uint32_t correct = 0;

    for (uint32_t i = 0; i < sample_count; i++)
    {
        float output[MAX_INPUT_SIZE];
        const float *input = &test_data[i * model->input_size];

        if (ml_infer(model_id, input, model->input_size, output, model->output_size) == 0)
        {
            float max_val = output[0];
            uint32_t pred_class = 0;

            for (uint32_t j = 1; j < model->output_size; j++)
            {
                if (output[j] > max_val)
                {
                    max_val = output[j];
                    pred_class = j;
                }
            }

            if (pred_class == (uint32_t)labels[i])
            {
                correct++;
            }
        }
    }

    return (float)correct / (float)sample_count * 100.0f;
}

int ml_quantize_model(uint32_t model_id)
{
    if (model_id == 0 || model_id > g_model_count)
    {
        return -1;
    }

    MLModel *model = &g_models[model_id - 1];
    model->version++;
    return 0;
}

int ml_prune_model(uint32_t model_id, float threshold)
{
    if (model_id == 0 || model_id > g_model_count)
    {
        return -1;
    }

    MLModel *model = &g_models[model_id - 1];
    uint32_t pruned_weights = 0;

    for (uint32_t i = 0; i < model->layer_count; i++)
    {
        NeuralLayer *layer = &model->layers[i];

        for (uint32_t j = 0; j < layer->input_size * layer->output_size; j++)
        {
            if (fabsf(layer->weights[j]) < threshold)
            {
                layer->weights[j] = 0.0f;
                pruned_weights++;
            }
        }
    }

    model->version++;
    return pruned_weights;
}

int ml_optimize_for_inference(uint32_t model_id)
{
    if (model_id == 0 || model_id > g_model_count)
    {
        return -1;
    }

    MLModel *model = &g_models[model_id - 1];
    model->version++;
    return 0;
}

MLModel *ml_get_model(uint32_t model_id)
{
    if (model_id == 0 || model_id > g_model_count)
    {
        return NULL;
    }

    return &g_models[model_id - 1];
}

uint32_t ml_get_model_count(void)
{
    return g_model_count;
}

uint64_t ml_get_total_inference_time(uint32_t model_id)
{
    if (model_id == 0 || model_id > g_model_count)
    {
        return 0;
    }

    return g_models[model_id - 1].total_inference_time_ms;
}

float ml_get_avg_inference_time(uint32_t model_id)
{
    if (model_id == 0 || model_id > g_model_count)
    {
        return 0.0f;
    }

    MLModel *model = &g_models[model_id - 1];
    if (model->inference_count == 0)
    {
        return 0.0f;
    }

    return (float)model->total_inference_time_ms / (float)model->inference_count;
}
