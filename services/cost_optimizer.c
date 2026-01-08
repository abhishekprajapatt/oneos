#include "cost_optimizer.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

static CostOptimizer g_optimizers[MAX_COST_OPTIMIZERS] = {0};
static uint32_t g_optimizer_count = 0;
static uint64_t g_billing_id_counter = 1;

int co_create_optimizer(const char *name, ChargebackModel chargeback_model)
{
    if (!name || g_optimizer_count >= MAX_COST_OPTIMIZERS)
        return -1;

    CostOptimizer *opt = &g_optimizers[g_optimizer_count];
    opt->optimizer_id = g_optimizer_count + 1;
    strncpy(opt->name, name, 63);
    opt->pricing_count = 0;
    opt->model_count = 0;
    opt->allocation_count = 0;
    opt->billing_count = 0;
    opt->rule_count = 0;
    opt->chargeback_model = chargeback_model;
    opt->total_revenue = 0.0;
    opt->total_costs = 0.0;
    opt->cost_savings = 0.0;
    opt->savings_percentage = 0.0;
    opt->total_billing_cycles = 0;
    opt->total_optimizations_performed = 0;
    opt->created_at = time(NULL);
    opt->last_billing_cycle = 0;
    opt->is_active = 1;

    g_optimizer_count++;
    return opt->optimizer_id;
}

int co_delete_optimizer(uint32_t optimizer_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
        return -1;

    for (uint32_t i = optimizer_id - 1; i < g_optimizer_count - 1; i++)
        g_optimizers[i] = g_optimizers[i + 1];

    g_optimizer_count--;
    return 0;
}

int co_add_resource_pricing(uint32_t optimizer_id, ResourceBillingType resource_type,
                            BillingModel billing_model, double unit_cost, const char *currency)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || !currency || unit_cost < 0)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    if (opt->pricing_count >= MAX_RESOURCE_PRICING)
        return -1;

    ResourcePricing *pricing = &opt->pricing[opt->pricing_count];
    pricing->pricing_id = opt->pricing_count + 1;
    pricing->resource_type = resource_type;
    pricing->billing_model = billing_model;
    pricing->unit_cost = unit_cost;
    strncpy(pricing->currency, currency, 3);
    pricing->effective_from = time(NULL);
    pricing->effective_to = 0;
    pricing->is_active = 1;

    opt->pricing_count++;
    return pricing->pricing_id;
}

int co_update_resource_pricing(uint32_t optimizer_id, uint32_t pricing_id, double unit_cost)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || unit_cost < 0)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    for (uint32_t i = 0; i < opt->pricing_count; i++)
    {
        if (opt->pricing[i].pricing_id == pricing_id)
        {
            opt->pricing[i].unit_cost = unit_cost;
            return 0;
        }
    }
    return -1;
}

int co_delete_resource_pricing(uint32_t optimizer_id, uint32_t pricing_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    for (uint32_t i = 0; i < opt->pricing_count; i++)
    {
        if (opt->pricing[i].pricing_id == pricing_id)
        {
            for (uint32_t j = i; j < opt->pricing_count - 1; j++)
                opt->pricing[j] = opt->pricing[j + 1];
            opt->pricing_count--;
            return 0;
        }
    }
    return -1;
}

int co_create_cost_model(uint32_t optimizer_id, const char *model_name,
                         ResourceBillingType resource_type, uint32_t pricing_id,
                         double base_cost, double variable_multiplier)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || !model_name)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    if (opt->model_count >= MAX_COST_MODELS)
        return -1;

    CostModel *model = &opt->cost_models[opt->model_count];
    model->model_id = opt->model_count + 1;
    strncpy(model->model_name, model_name, 63);
    model->resource_type = resource_type;
    model->pricing_id = pricing_id;
    model->base_cost = base_cost;
    model->variable_cost_multiplier = variable_multiplier;
    model->min_billing_unit = 1;
    model->include_discounts = 0;
    model->discount_percentage = 0.0;
    model->created_at = time(NULL);

    opt->model_count++;
    return model->model_id;
}

int co_delete_cost_model(uint32_t optimizer_id, uint32_t model_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    for (uint32_t i = 0; i < opt->model_count; i++)
    {
        if (opt->cost_models[i].model_id == model_id)
        {
            for (uint32_t j = i; j < opt->model_count - 1; j++)
                opt->cost_models[j] = opt->cost_models[j + 1];
            opt->model_count--;
            return 0;
        }
    }
    return -1;
}

int co_allocate_cost(uint32_t optimizer_id, uint32_t process_id, uint32_t owner_id,
                     ResourceBillingType resource_type, uint64_t resource_amount)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || resource_amount == 0)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    if (opt->allocation_count >= MAX_COST_ALLOCATIONS)
        return -1;

    CostAllocation *alloc = &opt->allocations[opt->allocation_count];
    alloc->allocation_id = opt->allocation_count + 1;
    alloc->process_id = process_id;
    alloc->owner_id = owner_id;
    alloc->resource_type = resource_type;
    alloc->resource_amount = resource_amount;
    alloc->cost_per_unit = 1.0;
    alloc->total_cost = resource_amount * 1.0;
    alloc->allocation_start = time(NULL);
    alloc->allocation_end = 0;
    alloc->allocation_duration_minutes = 0;
    alloc->is_active = 1;

    opt->allocation_count++;
    opt->total_costs += alloc->total_cost;
    return alloc->allocation_id;
}

int co_deallocate_cost(uint32_t optimizer_id, uint64_t allocation_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    for (uint32_t i = 0; i < opt->allocation_count; i++)
    {
        if (opt->allocations[i].allocation_id == allocation_id)
        {
            opt->allocations[i].is_active = 0;
            opt->allocations[i].allocation_end = time(NULL);
            return 0;
        }
    }
    return -1;
}

int co_get_allocations(uint32_t optimizer_id, CostAllocation *allocations, uint32_t *count)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || !allocations || !count)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    if (opt->allocation_count > *count)
        return -1;

    memcpy(allocations, opt->allocations, opt->allocation_count * sizeof(CostAllocation));
    *count = opt->allocation_count;
    return 0;
}

int co_create_billing_record(uint32_t optimizer_id, uint32_t owner_id, const char *owner_name)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || !owner_name)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    if (opt->billing_count >= MAX_BILLING_RECORDS)
        return -1;

    BillingRecord *record = &opt->billing_records[opt->billing_count];
    record->billing_id = g_billing_id_counter++;
    record->owner_id = owner_id;
    strncpy(record->owner_name, owner_name, 63);
    record->billing_period_start = time(NULL);
    record->billing_period_end = 0;
    record->total_cpu_cost = 0.0;
    record->total_memory_cost = 0.0;
    record->total_storage_cost = 0.0;
    record->total_network_cost = 0.0;
    record->total_gpu_cost = 0.0;
    record->total_cost = 0.0;
    record->applied_discounts = 0.0;
    record->final_cost = 0.0;
    record->is_finalized = 0;
    strncpy(record->payment_status, "pending", 31);

    opt->billing_count++;
    return record->billing_id;
}

int co_finalize_billing_record(uint32_t optimizer_id, uint64_t billing_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    for (uint32_t i = 0; i < opt->billing_count; i++)
    {
        if (opt->billing_records[i].billing_id == billing_id)
        {
            opt->billing_records[i].is_finalized = 1;
            opt->billing_records[i].billing_period_end = time(NULL);
            opt->total_revenue += opt->billing_records[i].final_cost;
            opt->total_billing_cycles++;
            return 0;
        }
    }
    return -1;
}

int co_get_billing_records(uint32_t optimizer_id, BillingRecord *records, uint32_t *count)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || !records || !count)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    if (opt->billing_count > *count)
        return -1;

    memcpy(records, opt->billing_records, opt->billing_count * sizeof(BillingRecord));
    *count = opt->billing_count;
    return 0;
}

int co_add_optimization_rule(uint32_t optimizer_id, const char *rule_name,
                             CostOptimizationStrategy strategy, double cost_threshold,
                             double target_reduction)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || !rule_name)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    if (opt->rule_count >= MAX_COST_OPTIMIZATION_RULES)
        return -1;

    CostOptimizationRule *rule = &opt->optimization_rules[opt->rule_count];
    rule->rule_id = opt->rule_count + 1;
    strncpy(rule->rule_name, rule_name, 63);
    rule->strategy = strategy;
    rule->cost_threshold = cost_threshold;
    rule->target_cost_reduction = target_reduction;
    rule->evaluation_interval_hours = 24;
    rule->last_evaluation = 0;
    rule->actions_taken = 0;
    rule->is_enabled = 1;

    opt->rule_count++;
    return rule->rule_id;
}

int co_trigger_optimization_rule(uint32_t optimizer_id, uint32_t rule_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    for (uint32_t i = 0; i < opt->rule_count; i++)
    {
        if (opt->optimization_rules[i].rule_id == rule_id)
        {
            opt->optimization_rules[i].actions_taken++;
            opt->optimization_rules[i].last_evaluation = time(NULL);
            opt->total_optimizations_performed++;
            return 0;
        }
    }
    return -1;
}

int co_delete_optimization_rule(uint32_t optimizer_id, uint32_t rule_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    for (uint32_t i = 0; i < opt->rule_count; i++)
    {
        if (opt->optimization_rules[i].rule_id == rule_id)
        {
            for (uint32_t j = i; j < opt->rule_count - 1; j++)
                opt->optimization_rules[j] = opt->optimization_rules[j + 1];
            opt->rule_count--;
            return 0;
        }
    }
    return -1;
}

int co_calculate_cost_for_resource(uint32_t optimizer_id, ResourceBillingType resource_type,
                                   uint64_t resource_amount, double *cost)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || !cost)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    *cost = 0.0;

    for (uint32_t i = 0; i < opt->pricing_count; i++)
    {
        if (opt->pricing[i].resource_type == resource_type && opt->pricing[i].is_active)
        {
            *cost = resource_amount * opt->pricing[i].unit_cost;
            return 0;
        }
    }
    return -1;
}

int co_calculate_total_cost(uint32_t optimizer_id, uint32_t owner_id, double *total_cost)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || !total_cost)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    *total_cost = 0.0;

    for (uint32_t i = 0; i < opt->allocation_count; i++)
    {
        if (opt->allocations[i].owner_id == owner_id && opt->allocations[i].is_active)
            *total_cost += opt->allocations[i].total_cost;
    }
    return 0;
}

int co_estimate_cost_for_operation(uint32_t optimizer_id, uint64_t resource_amount,
                                   uint32_t duration_minutes, double *estimated_cost)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || !estimated_cost || duration_minutes == 0)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    *estimated_cost = 0.0;

    if (opt->model_count > 0)
    {
        CostModel *model = &opt->cost_models[0];
        *estimated_cost = (model->base_cost + (resource_amount * model->variable_cost_multiplier)) *
                          (duration_minutes / 60.0);
    }
    return 0;
}

int co_get_cost_breakdown(uint32_t optimizer_id, uint32_t owner_id,
                          double *cpu_cost, double *memory_cost, double *storage_cost,
                          double *network_cost, double *gpu_cost)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
        return -1;

    *cpu_cost = *memory_cost = *storage_cost = *network_cost = *gpu_cost = 0.0;
    return 0;
}

uint64_t co_get_total_revenue(uint32_t optimizer_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
        return 0;
    return (uint64_t)g_optimizers[optimizer_id - 1].total_revenue;
}

uint64_t co_get_total_costs(uint32_t optimizer_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
        return 0;
    return (uint64_t)g_optimizers[optimizer_id - 1].total_costs;
}

uint64_t co_get_cost_savings(uint32_t optimizer_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
        return 0;
    return (uint64_t)g_optimizers[optimizer_id - 1].cost_savings;
}

double co_get_savings_percentage(uint32_t optimizer_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
        return 0.0;
    return g_optimizers[optimizer_id - 1].savings_percentage;
}

int co_perform_cost_optimization(uint32_t optimizer_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    opt->total_optimizations_performed++;
    return 0;
}

int co_forecast_costs(uint32_t optimizer_id, uint32_t days_ahead, double *forecasted_cost)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count || !forecasted_cost || days_ahead == 0)
        return -1;

    CostOptimizer *opt = &g_optimizers[optimizer_id - 1];
    double daily_cost = opt->total_costs / 30.0;
    *forecasted_cost = daily_cost * days_ahead;
    return 0;
}

CostOptimizer *co_get_optimizer_info(uint32_t optimizer_id)
{
    if (optimizer_id == 0 || optimizer_id > g_optimizer_count)
        return NULL;
    return &g_optimizers[optimizer_id - 1];
}
