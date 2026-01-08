#ifndef ONEOS_COST_OPTIMIZER_H
#define ONEOS_COST_OPTIMIZER_H

#include <stdint.h>
#include <time.h>

#define MAX_COST_OPTIMIZERS 16
#define MAX_COST_MODELS 64
#define MAX_RESOURCE_PRICING 256
#define MAX_COST_ALLOCATIONS 2048
#define MAX_BILLING_RECORDS 8192
#define MAX_COST_OPTIMIZATION_RULES 512

typedef enum
{
    RESOURCE_BILLING_CPU = 0,
    RESOURCE_BILLING_MEMORY = 1,
    RESOURCE_BILLING_STORAGE = 2,
    RESOURCE_BILLING_NETWORK = 3,
    RESOURCE_BILLING_GPU = 4,
    RESOURCE_BILLING_LICENSE = 5
} ResourceBillingType;

typedef enum
{
    BILLING_MODEL_PER_HOUR = 0,
    BILLING_MODEL_PER_MINUTE = 1,
    BILLING_MODEL_PER_GB = 2,
    BILLING_MODEL_PER_OPERATION = 3,
    BILLING_MODEL_FLAT_RATE = 4,
    BILLING_MODEL_TIERED = 5
} BillingModel;

typedef enum
{
    COST_OPTIMIZATION_IDLE_DETECTION = 0,
    COST_OPTIMIZATION_RIGHT_SIZING = 1,
    COST_OPTIMIZATION_RESOURCE_CONSOLIDATION = 2,
    COST_OPTIMIZATION_DEMAND_FORECASTING = 3,
    COST_OPTIMIZATION_ANOMALY_DETECTION = 4
} CostOptimizationStrategy;

typedef enum
{
    CHARGEBACK_MODEL_DIRECT = 0,
    CHARGEBACK_MODEL_SHARED = 1,
    CHARGEBACK_MODEL_WEIGHTED = 2,
    CHARGEBACK_MODEL_CUSTOM = 3
} ChargebackModel;

typedef struct
{
    uint32_t pricing_id;
    ResourceBillingType resource_type;
    BillingModel billing_model;
    double unit_cost;
    char currency[4];
    time_t effective_from;
    time_t effective_to;
    uint8_t is_active;
} ResourcePricing;

typedef struct
{
    uint32_t model_id;
    char model_name[64];
    ResourceBillingType resource_type;
    uint32_t pricing_id;
    double base_cost;
    double variable_cost_multiplier;
    uint32_t min_billing_unit;
    uint8_t include_discounts;
    double discount_percentage;
    time_t created_at;
} CostModel;

typedef struct
{
    uint64_t allocation_id;
    uint32_t process_id;
    uint32_t owner_id;
    ResourceBillingType resource_type;
    uint64_t resource_amount;
    double cost_per_unit;
    double total_cost;
    time_t allocation_start;
    time_t allocation_end;
    uint32_t allocation_duration_minutes;
    uint8_t is_active;
} CostAllocation;

typedef struct
{
    uint64_t billing_id;
    uint32_t owner_id;
    char owner_name[64];
    time_t billing_period_start;
    time_t billing_period_end;
    double total_cpu_cost;
    double total_memory_cost;
    double total_storage_cost;
    double total_network_cost;
    double total_gpu_cost;
    double total_cost;
    double applied_discounts;
    double final_cost;
    uint8_t is_finalized;
    char payment_status[32];
} BillingRecord;

typedef struct
{
    uint32_t rule_id;
    char rule_name[64];
    CostOptimizationStrategy strategy;
    double cost_threshold;
    double target_cost_reduction;
    uint32_t evaluation_interval_hours;
    time_t last_evaluation;
    uint32_t actions_taken;
    uint8_t is_enabled;
} CostOptimizationRule;

typedef struct
{
    uint32_t optimizer_id;
    char name[64];

    uint32_t pricing_count;
    ResourcePricing pricing[MAX_RESOURCE_PRICING];

    uint32_t model_count;
    CostModel cost_models[MAX_COST_MODELS];

    uint32_t allocation_count;
    CostAllocation allocations[MAX_COST_ALLOCATIONS];

    uint32_t billing_count;
    BillingRecord billing_records[MAX_BILLING_RECORDS];

    uint32_t rule_count;
    CostOptimizationRule optimization_rules[MAX_COST_OPTIMIZATION_RULES];

    ChargebackModel chargeback_model;
    double total_revenue;
    double total_costs;
    double cost_savings;
    double savings_percentage;

    uint64_t total_billing_cycles;
    uint64_t total_optimizations_performed;

    time_t created_at;
    time_t last_billing_cycle;
    uint8_t is_active;
} CostOptimizer;

int co_create_optimizer(const char *name, ChargebackModel chargeback_model);
int co_delete_optimizer(uint32_t optimizer_id);

int co_add_resource_pricing(uint32_t optimizer_id, ResourceBillingType resource_type,
                            BillingModel billing_model, double unit_cost, const char *currency);
int co_update_resource_pricing(uint32_t optimizer_id, uint32_t pricing_id, double unit_cost);
int co_delete_resource_pricing(uint32_t optimizer_id, uint32_t pricing_id);

int co_create_cost_model(uint32_t optimizer_id, const char *model_name,
                         ResourceBillingType resource_type, uint32_t pricing_id,
                         double base_cost, double variable_multiplier);
int co_delete_cost_model(uint32_t optimizer_id, uint32_t model_id);

int co_allocate_cost(uint32_t optimizer_id, uint32_t process_id, uint32_t owner_id,
                     ResourceBillingType resource_type, uint64_t resource_amount);
int co_deallocate_cost(uint32_t optimizer_id, uint64_t allocation_id);
int co_get_allocations(uint32_t optimizer_id, CostAllocation *allocations, uint32_t *count);

int co_create_billing_record(uint32_t optimizer_id, uint32_t owner_id, const char *owner_name);
int co_finalize_billing_record(uint32_t optimizer_id, uint64_t billing_id);
int co_get_billing_records(uint32_t optimizer_id, BillingRecord *records, uint32_t *count);

int co_add_optimization_rule(uint32_t optimizer_id, const char *rule_name,
                             CostOptimizationStrategy strategy, double cost_threshold,
                             double target_reduction);
int co_trigger_optimization_rule(uint32_t optimizer_id, uint32_t rule_id);
int co_delete_optimization_rule(uint32_t optimizer_id, uint32_t rule_id);

int co_calculate_cost_for_resource(uint32_t optimizer_id, ResourceBillingType resource_type,
                                   uint64_t resource_amount, double *cost);
int co_calculate_total_cost(uint32_t optimizer_id, uint32_t owner_id, double *total_cost);
int co_estimate_cost_for_operation(uint32_t optimizer_id, uint64_t resource_amount,
                                   uint32_t duration_minutes, double *estimated_cost);

int co_get_cost_breakdown(uint32_t optimizer_id, uint32_t owner_id,
                          double *cpu_cost, double *memory_cost, double *storage_cost,
                          double *network_cost, double *gpu_cost);

uint64_t co_get_total_revenue(uint32_t optimizer_id);
uint64_t co_get_total_costs(uint32_t optimizer_id);
uint64_t co_get_cost_savings(uint32_t optimizer_id);
double co_get_savings_percentage(uint32_t optimizer_id);

int co_perform_cost_optimization(uint32_t optimizer_id);
int co_forecast_costs(uint32_t optimizer_id, uint32_t days_ahead, double *forecasted_cost);

CostOptimizer *co_get_optimizer_info(uint32_t optimizer_id);

#endif
