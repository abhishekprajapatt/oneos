#ifndef ML_OPTIMIZER_H
#define ML_OPTIMIZER_H

#include <stdint.h>
#include <stddef.h>
#include <math.h>

#define MAX_OPTIMIZATION_JOBS 256
#define MAX_OPTIMIZATION_ITERATIONS 10000
#define MAX_HYPERPARAMETERS 128
#define MAX_LEARNING_RATE_SCHEDULES 16

typedef enum
{
    OPTIMIZER_SGD,
    OPTIMIZER_MOMENTUM,
    OPTIMIZER_NESTEROV,
    OPTIMIZER_ADAGRAD,
    OPTIMIZER_ADAM,
    OPTIMIZER_ADAMW,
    OPTIMIZER_RMSPROP,
    OPTIMIZER_ADADELTA
} OptimizerType;

typedef enum
{
    LR_SCHEDULE_CONSTANT,
    LR_SCHEDULE_LINEAR_DECAY,
    LR_SCHEDULE_EXPONENTIAL_DECAY,
    LR_SCHEDULE_STEP_DECAY,
    LR_SCHEDULE_COSINE_ANNEALING,
    LR_SCHEDULE_WARMUP_COSINE
} LearningRateScheduleType;

typedef struct
{
    char param_name[64];
    float value;
    float min_value;
    float max_value;
    uint32_t step_count;
} Hyperparameter;

typedef struct
{
    uint32_t schedule_id;
    LearningRateScheduleType type;

    float initial_learning_rate;
    float final_learning_rate;
    uint32_t decay_steps;
    float decay_rate;

    float warmup_steps;
    uint32_t total_steps;
    uint32_t current_step;
} LearningRateSchedule;

typedef struct
{
    uint32_t optimization_job_id;
    OptimizerType optimizer_type;

    float learning_rate;
    float momentum;
    float dampening;
    float weight_decay;

    float beta1; // Adam parameter
    float beta2; // Adam parameter
    float epsilon;

    uint32_t current_iteration;
    uint32_t max_iterations;

    float current_loss;
    float best_loss;
    uint32_t best_iteration;

    uint32_t hyperparameter_count;
    Hyperparameter hyperparameters[MAX_HYPERPARAMETERS];

    LearningRateSchedule *lr_schedule;

    uint64_t optimization_time_ms;
    float gradient_norm;
} OptimizationJob;

typedef struct
{
    uint32_t optimizer_id;

    uint32_t job_count;
    OptimizationJob jobs[MAX_OPTIMIZATION_JOBS];

    uint32_t schedule_count;
    LearningRateSchedule schedules[MAX_LEARNING_RATE_SCHEDULES];

    uint64_t total_training_steps;
    uint64_t total_optimization_time_ms;

    float average_convergence_rate;
    float global_gradient_norm;
} MLOptimizer;

// Optimizer initialization
MLOptimizer *ml_optimizer_init(void);
int ml_optimizer_destroy(MLOptimizer *opt);

// Optimization job management
OptimizationJob *ml_optimizer_create_job(MLOptimizer *opt, OptimizerType type);
int ml_optimizer_destroy_job(MLOptimizer *opt, uint32_t job_id);

// Optimizer configuration
int ml_optimizer_set_learning_rate(OptimizationJob *job, float lr);
int ml_optimizer_set_momentum(OptimizationJob *job, float momentum);
int ml_optimizer_set_weight_decay(OptimizationJob *job, float wd);

// Hyperparameter management
int ml_optimizer_add_hyperparameter(OptimizationJob *job, const char *name, float value, float min, float max);
int ml_optimizer_update_hyperparameter(OptimizationJob *job, const char *name, float value);
int ml_optimizer_get_hyperparameter(OptimizationJob *job, const char *name, float *value);

// Learning rate scheduling
LearningRateSchedule *ml_optimizer_create_lr_schedule(MLOptimizer *opt, LearningRateScheduleType type,
                                                      float initial_lr, uint32_t total_steps);
int ml_optimizer_update_lr_schedule(OptimizationJob *job, uint32_t step);
float ml_optimizer_get_current_lr(OptimizationJob *job);

// Optimization step
int ml_optimizer_step(OptimizationJob *job, float loss, const float *gradients, uint32_t gradient_count);
int ml_optimizer_zero_grad(OptimizationJob *job);

// Gradient operations
int ml_optimizer_compute_gradient_norm(OptimizationJob *job, const float *gradients, uint32_t count);
int ml_optimizer_apply_gradient_clipping(OptimizationJob *job, float *gradients, uint32_t count, float max_norm);

// Convergence tracking
int ml_optimizer_check_convergence(OptimizationJob *job);
int ml_optimizer_get_convergence_info(OptimizationJob *job, float *loss_change, uint32_t *iterations_since_best);

// Learning rate adjustment
int ml_optimizer_adjust_learning_rate(OptimizationJob *job, float multiplier);
int ml_optimizer_reduce_learning_rate_on_plateau(MLOptimizer *opt, uint32_t job_id, uint32_t patience);

// Monitoring
typedef struct
{
    uint32_t job_id;
    OptimizerType optimizer_type;
    float current_learning_rate;
    float current_loss;
    float best_loss;
    uint32_t total_iterations;
    float convergence_rate;
    float estimated_time_to_convergence;
} OptimizationMetrics;

int ml_optimizer_get_metrics(OptimizationJob *job, OptimizationMetrics *metrics);
int ml_optimizer_get_global_metrics(MLOptimizer *opt);

// Checkpoint management
int ml_optimizer_save_checkpoint(OptimizationJob *job, const char *path);
int ml_optimizer_load_checkpoint(OptimizationJob *job, const char *path);

#endif // ML_OPTIMIZER_H
