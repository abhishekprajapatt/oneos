#include "ml_optimizer.h"
#include <stdlib.h>
#include <string.h>

static MLOptimizer *global_optimizer = NULL;
static uint32_t next_job_id = 1;
static uint32_t next_schedule_id = 1;

MLOptimizer *ml_optimizer_init(void)
{
    MLOptimizer *opt = (MLOptimizer *)malloc(sizeof(MLOptimizer));
    if (!opt)
        return NULL;

    memset(opt, 0, sizeof(MLOptimizer));
    opt->optimizer_id = 1;
    opt->job_count = 0;
    opt->schedule_count = 0;
    opt->total_training_steps = 0;
    opt->total_optimization_time_ms = 0;
    opt->average_convergence_rate = 0.0f;
    opt->global_gradient_norm = 0.0f;

    global_optimizer = opt;
    return opt;
}

int ml_optimizer_destroy(MLOptimizer *opt)
{
    if (!opt)
        return -1;

    for (uint32_t i = 0; i < opt->job_count; i++)
    {
        if (opt->jobs[i].lr_schedule)
            free(opt->jobs[i].lr_schedule);
    }

    free(opt);
    global_optimizer = NULL;
    return 0;
}

OptimizationJob *ml_optimizer_create_job(MLOptimizer *opt, OptimizerType type)
{
    if (!opt || opt->job_count >= MAX_OPTIMIZATION_JOBS)
        return NULL;

    OptimizationJob *job = &opt->jobs[opt->job_count];
    memset(job, 0, sizeof(OptimizationJob));

    job->optimization_job_id = next_job_id++;
    job->optimizer_type = type;

    // Set default hyperparameters based on optimizer type
    job->learning_rate = 0.001f;
    job->momentum = 0.9f;
    job->dampening = 0.0f;
    job->weight_decay = 0.0001f;
    job->beta1 = 0.9f;   // Adam
    job->beta2 = 0.999f; // Adam
    job->epsilon = 1e-8f;

    job->current_iteration = 0;
    job->max_iterations = MAX_OPTIMIZATION_ITERATIONS;
    job->current_loss = 0.0f;
    job->best_loss = 1e10f;
    job->best_iteration = 0;
    job->hyperparameter_count = 0;
    job->lr_schedule = NULL;
    job->optimization_time_ms = 0;
    job->gradient_norm = 0.0f;

    opt->job_count++;
    return job;
}

int ml_optimizer_destroy_job(MLOptimizer *opt, uint32_t job_id)
{
    if (!opt)
        return -1;

    for (uint32_t i = 0; i < opt->job_count; i++)
    {
        if (opt->jobs[i].optimization_job_id == job_id)
        {
            if (opt->jobs[i].lr_schedule)
                free(opt->jobs[i].lr_schedule);
            return 0;
        }
    }

    return -1;
}

int ml_optimizer_set_learning_rate(OptimizationJob *job, float lr)
{
    if (!job || lr <= 0.0f)
        return -1;

    job->learning_rate = lr;
    return 0;
}

int ml_optimizer_set_momentum(OptimizationJob *job, float momentum)
{
    if (!job || momentum < 0.0f || momentum >= 1.0f)
        return -1;

    job->momentum = momentum;
    return 0;
}

int ml_optimizer_set_weight_decay(OptimizationJob *job, float wd)
{
    if (!job || wd < 0.0f)
        return -1;

    job->weight_decay = wd;
    return 0;
}

int ml_optimizer_add_hyperparameter(OptimizationJob *job, const char *name, float value, float min, float max)
{
    if (!job || job->hyperparameter_count >= MAX_HYPERPARAMETERS || !name)
        return -1;

    Hyperparameter *hp = &job->hyperparameters[job->hyperparameter_count];
    strncpy(hp->param_name, name, 63);
    hp->value = value;
    hp->min_value = min;
    hp->max_value = max;
    hp->step_count = 0;

    job->hyperparameter_count++;
    return 0;
}

int ml_optimizer_update_hyperparameter(OptimizationJob *job, const char *name, float value)
{
    if (!job || !name)
        return -1;

    for (uint32_t i = 0; i < job->hyperparameter_count; i++)
    {
        if (strcmp(job->hyperparameters[i].param_name, name) == 0)
        {
            if (value < job->hyperparameters[i].min_value || value > job->hyperparameters[i].max_value)
                return -1;
            job->hyperparameters[i].value = value;
            return 0;
        }
    }

    return -1;
}

int ml_optimizer_get_hyperparameter(OptimizationJob *job, const char *name, float *value)
{
    if (!job || !name || !value)
        return -1;

    for (uint32_t i = 0; i < job->hyperparameter_count; i++)
    {
        if (strcmp(job->hyperparameters[i].param_name, name) == 0)
        {
            *value = job->hyperparameters[i].value;
            return 0;
        }
    }

    return -1;
}

LearningRateSchedule *ml_optimizer_create_lr_schedule(MLOptimizer *opt, LearningRateScheduleType type,
                                                      float initial_lr, uint32_t total_steps)
{
    if (!opt || opt->schedule_count >= MAX_LEARNING_RATE_SCHEDULES || initial_lr <= 0.0f)
        return NULL;

    LearningRateSchedule *schedule = &opt->schedules[opt->schedule_count];
    memset(schedule, 0, sizeof(LearningRateSchedule));

    schedule->schedule_id = next_schedule_id++;
    schedule->type = type;
    schedule->initial_learning_rate = initial_lr;
    schedule->final_learning_rate = initial_lr * 0.01f;
    schedule->decay_steps = total_steps;
    schedule->decay_rate = 0.96f;
    schedule->warmup_steps = 0;
    schedule->total_steps = total_steps;
    schedule->current_step = 0;

    opt->schedule_count++;
    return schedule;
}

int ml_optimizer_update_lr_schedule(OptimizationJob *job, uint32_t step)
{
    if (!job || !job->lr_schedule)
        return -1;

    job->lr_schedule->current_step = step;
    return 0;
}

float ml_optimizer_get_current_lr(OptimizationJob *job)
{
    if (!job)
        return 0.0f;

    if (!job->lr_schedule)
        return job->learning_rate;

    LearningRateSchedule *schedule = job->lr_schedule;
    float progress = (float)schedule->current_step / schedule->total_steps;

    switch (schedule->type)
    {
    case LR_SCHEDULE_CONSTANT:
        return schedule->initial_learning_rate;

    case LR_SCHEDULE_LINEAR_DECAY:
        return schedule->initial_learning_rate * (1.0f - progress);

    case LR_SCHEDULE_EXPONENTIAL_DECAY:
        return schedule->initial_learning_rate * powf(schedule->decay_rate, progress * 100.0f);

    case LR_SCHEDULE_COSINE_ANNEALING:
        return schedule->final_learning_rate + (schedule->initial_learning_rate - schedule->final_learning_rate) *
                                                   (1.0f + cosf(3.14159f * progress)) / 2.0f;

    default:
        return schedule->initial_learning_rate;
    }
}

int ml_optimizer_step(OptimizationJob *job, float loss, const float *gradients, uint32_t gradient_count)
{
    if (!job || !gradients || gradient_count == 0)
        return -1;

    job->current_iteration++;
    job->current_loss = loss;

    if (loss < job->best_loss)
    {
        job->best_loss = loss;
        job->best_iteration = job->current_iteration;
    }

    // Update learning rate if schedule exists
    if (job->lr_schedule)
    {
        job->learning_rate = ml_optimizer_get_current_lr(job);
    }

    return 0;
}

int ml_optimizer_zero_grad(OptimizationJob *job)
{
    if (!job)
        return -1;

    job->gradient_norm = 0.0f;
    return 0;
}

int ml_optimizer_compute_gradient_norm(OptimizationJob *job, const float *gradients, uint32_t count)
{
    if (!job || !gradients || count == 0)
        return -1;

    job->gradient_norm = 0.0f;
    for (uint32_t i = 0; i < count; i++)
    {
        job->gradient_norm += gradients[i] * gradients[i];
    }

    job->gradient_norm = sqrtf(job->gradient_norm);
    return 0;
}

int ml_optimizer_apply_gradient_clipping(OptimizationJob *job, float *gradients, uint32_t count, float max_norm)
{
    if (!job || !gradients || count == 0 || max_norm <= 0.0f)
        return -1;

    float current_norm = 0.0f;
    for (uint32_t i = 0; i < count; i++)
    {
        current_norm += gradients[i] * gradients[i];
    }
    current_norm = sqrtf(current_norm);

    if (current_norm > max_norm)
    {
        float scale = max_norm / (current_norm + 1e-8f);
        for (uint32_t i = 0; i < count; i++)
        {
            gradients[i] *= scale;
        }
    }

    return 0;
}

int ml_optimizer_check_convergence(OptimizationJob *job)
{
    if (!job)
        return -1;

    // Check if loss hasn't improved in last 100 iterations
    if (job->current_iteration - job->best_iteration > 100)
        return 1; // Converged

    return 0;
}

int ml_optimizer_get_convergence_info(OptimizationJob *job, float *loss_change, uint32_t *iterations_since_best)
{
    if (!job || !loss_change || !iterations_since_best)
        return -1;

    *loss_change = fabsf(job->best_loss - job->current_loss);
    *iterations_since_best = job->current_iteration - job->best_iteration;

    return 0;
}

int ml_optimizer_adjust_learning_rate(OptimizationJob *job, float multiplier)
{
    if (!job || multiplier <= 0.0f)
        return -1;

    job->learning_rate *= multiplier;
    return 0;
}

int ml_optimizer_reduce_learning_rate_on_plateau(MLOptimizer *opt, uint32_t job_id, uint32_t patience)
{
    if (!opt)
        return -1;

    for (uint32_t i = 0; i < opt->job_count; i++)
    {
        if (opt->jobs[i].optimization_job_id == job_id)
        {
            if (opt->jobs[i].current_iteration - opt->jobs[i].best_iteration > patience)
            {
                ml_optimizer_adjust_learning_rate(&opt->jobs[i], 0.5f);
                return 0;
            }
        }
    }

    return -1;
}

int ml_optimizer_get_metrics(OptimizationJob *job, OptimizationMetrics *metrics)
{
    if (!job || !metrics)
        return -1;

    metrics->job_id = job->optimization_job_id;
    metrics->optimizer_type = job->optimizer_type;
    metrics->current_learning_rate = job->learning_rate;
    metrics->current_loss = job->current_loss;
    metrics->best_loss = job->best_loss;
    metrics->total_iterations = job->current_iteration;

    if (job->best_loss > 0.0f)
    {
        metrics->convergence_rate = (job->best_loss - job->current_loss) / job->best_loss;
    }
    else
    {
        metrics->convergence_rate = 0.0f;
    }

    if (metrics->convergence_rate > 0.0f)
    {
        metrics->estimated_time_to_convergence = (uint32_t)(job->current_iteration / metrics->convergence_rate);
    }
    else
    {
        metrics->estimated_time_to_convergence = 0.0f;
    }

    return 0;
}

int ml_optimizer_get_global_metrics(MLOptimizer *opt)
{
    if (!opt)
        return -1;

    opt->total_training_steps = 0;
    for (uint32_t i = 0; i < opt->job_count; i++)
    {
        opt->total_training_steps += opt->jobs[i].current_iteration;
    }

    return 0;
}

int ml_optimizer_save_checkpoint(OptimizationJob *job, const char *path)
{
    if (!job || !path)
        return -1;

    // Simulate checkpoint save
    return 0;
}

int ml_optimizer_load_checkpoint(OptimizationJob *job, const char *path)
{
    if (!job || !path)
        return -1;

    // Simulate checkpoint load
    return 0;
}
