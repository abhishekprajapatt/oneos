#include "system_validator.h"
#include <stdlib.h>
#include <string.h>

static SystemValidator *global_validator = NULL;
static uint32_t next_check_id = 1;
static uint32_t next_rule_id = 1;

SystemValidator *system_validator_init(void)
{
    SystemValidator *validator = (SystemValidator *)malloc(sizeof(SystemValidator));
    if (!validator)
        return NULL;

    memset(validator, 0, sizeof(SystemValidator));
    validator->validator_id = 1;
    validator->check_count = 0;
    validator->components_passed = 0;
    validator->components_failed = 0;
    validator->validation_start_time = 0;
    validator->validation_end_time = 0;

    global_validator = validator;
    return validator;
}

int system_validator_destroy(SystemValidator *validator)
{
    if (!validator)
        return -1;

    free(validator);
    global_validator = NULL;
    return 0;
}

uint32_t component_check_create(SystemValidator *validator, const char *component_name)
{
    if (!validator || validator->check_count >= MAX_COMPONENT_CHECKS || !component_name)
        return 0;

    ComponentCheck *check = &validator->checks[validator->check_count];
    memset(check, 0, sizeof(ComponentCheck));

    check->check_id = next_check_id++;
    strncpy(check->component_name, component_name, sizeof(check->component_name) - 1);

    check->rule_count = 0;
    check->error_count = 0;
    check->passed_rules = 0;
    check->failed_rules = 0;
    check->is_valid = 1;

    validator->check_count++;

    return check->check_id;
}

int component_check_destroy(SystemValidator *validator, uint32_t check_id)
{
    if (!validator || check_id == 0)
        return -1;

    for (uint32_t i = 0; i < validator->check_count; i++)
    {
        if (validator->checks[i].check_id == check_id)
        {
            memmove(&validator->checks[i], &validator->checks[i + 1],
                    sizeof(ComponentCheck) * (validator->check_count - i - 1));
            validator->check_count--;
            return 0;
        }
    }

    return -1;
}

uint32_t validation_rule_create(SystemValidator *validator, uint32_t check_id,
                                const char *rule_name, ValidationRuleType type)
{
    if (!validator || check_id == 0 || !rule_name)
        return 0;

    for (uint32_t i = 0; i < validator->check_count; i++)
    {
        if (validator->checks[i].check_id == check_id)
        {
            ComponentCheck *check = &validator->checks[i];

            if (check->rule_count >= MAX_VALIDATION_RULES)
                return 0;

            ValidationRule *rule = &check->rules[check->rule_count];
            memset(rule, 0, sizeof(ValidationRule));

            rule->rule_id = next_rule_id++;
            strncpy(rule->rule_name, rule_name, sizeof(rule->rule_name) - 1);
            rule->rule_type = type;

            rule->is_critical = 0;
            rule->is_enabled = 1;

            check->rule_count++;
            return rule->rule_id;
        }
    }

    return 0;
}

int validation_rule_enable(SystemValidator *validator, uint32_t check_id, uint32_t rule_id)
{
    if (!validator || check_id == 0 || rule_id == 0)
        return -1;

    for (uint32_t i = 0; i < validator->check_count; i++)
    {
        if (validator->checks[i].check_id == check_id)
        {
            ComponentCheck *check = &validator->checks[i];

            for (uint32_t j = 0; j < check->rule_count; j++)
            {
                if (check->rules[j].rule_id == rule_id)
                {
                    check->rules[j].is_enabled = 1;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int validation_rule_disable(SystemValidator *validator, uint32_t check_id, uint32_t rule_id)
{
    if (!validator || check_id == 0 || rule_id == 0)
        return -1;

    for (uint32_t i = 0; i < validator->check_count; i++)
    {
        if (validator->checks[i].check_id == check_id)
        {
            ComponentCheck *check = &validator->checks[i];

            for (uint32_t j = 0; j < check->rule_count; j++)
            {
                if (check->rules[j].rule_id == rule_id)
                {
                    check->rules[j].is_enabled = 0;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int validation_rule_mark_critical(SystemValidator *validator, uint32_t check_id,
                                  uint32_t rule_id)
{
    if (!validator || check_id == 0 || rule_id == 0)
        return -1;

    for (uint32_t i = 0; i < validator->check_count; i++)
    {
        if (validator->checks[i].check_id == check_id)
        {
            ComponentCheck *check = &validator->checks[i];

            for (uint32_t j = 0; j < check->rule_count; j++)
            {
                if (check->rules[j].rule_id == rule_id)
                {
                    check->rules[j].is_critical = 1;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int component_validation_run(SystemValidator *validator, uint32_t check_id)
{
    if (!validator || check_id == 0)
        return -1;

    for (uint32_t i = 0; i < validator->check_count; i++)
    {
        if (validator->checks[i].check_id == check_id)
        {
            ComponentCheck *check = &validator->checks[i];

            check->passed_rules = 0;
            check->failed_rules = 0;

            for (uint32_t j = 0; j < check->rule_count; j++)
            {
                if (check->rules[j].is_enabled)
                {
                    check->passed_rules++;
                }
            }

            if (check->failed_rules == 0)
            {
                check->is_valid = 1;
                validator->components_passed++;
            }
            else
            {
                check->is_valid = 0;
                validator->components_failed++;
            }

            return 0;
        }
    }

    return -1;
}

int component_validation_run_all(SystemValidator *validator)
{
    if (!validator)
        return -1;

    for (uint32_t i = 0; i < validator->check_count; i++)
    {
        component_validation_run(validator, validator->checks[i].check_id);
    }

    return 0;
}

int validation_add_error(SystemValidator *validator, uint32_t check_id,
                         ValidationErrorLevel level, const char *error_message,
                         uint32_t component_id)
{
    if (!validator || check_id == 0 || !error_message)
        return -1;

    for (uint32_t i = 0; i < validator->check_count; i++)
    {
        if (validator->checks[i].check_id == check_id)
        {
            ComponentCheck *check = &validator->checks[i];

            if (check->error_count >= MAX_VALIDATION_ERRORS)
                return -1;

            ValidationError *error = &check->errors[check->error_count];
            memset(error, 0, sizeof(ValidationError));

            error->error_id = check->error_count + 1;
            error->level = level;
            strncpy(error->error_message, error_message, sizeof(error->error_message) - 1);
            error->component_id = component_id;
            error->timestamp = 0;

            check->error_count++;

            if (level == ERROR_LEVEL_CRITICAL || level == ERROR_LEVEL_ERROR)
            {
                check->is_valid = 0;
            }

            return 0;
        }
    }

    return -1;
}

uint32_t validation_get_error_count(SystemValidator *validator, uint32_t check_id)
{
    if (!validator || check_id == 0)
        return 0;

    for (uint32_t i = 0; i < validator->check_count; i++)
    {
        if (validator->checks[i].check_id == check_id)
        {
            return validator->checks[i].error_count;
        }
    }

    return 0;
}

uint32_t validation_get_error_count_by_level(SystemValidator *validator, uint32_t check_id,
                                             ValidationErrorLevel level)
{
    if (!validator || check_id == 0)
        return 0;

    for (uint32_t i = 0; i < validator->check_count; i++)
    {
        if (validator->checks[i].check_id == check_id)
        {
            ComponentCheck *check = &validator->checks[i];
            uint32_t count = 0;

            for (uint32_t j = 0; j < check->error_count; j++)
            {
                if (check->errors[j].level == level)
                    count++;
            }

            return count;
        }
    }

    return 0;
}

int validation_get_results(SystemValidator *validator, uint32_t check_id,
                           uint32_t *passed, uint32_t *failed)
{
    if (!validator || check_id == 0 || !passed || !failed)
        return -1;

    for (uint32_t i = 0; i < validator->check_count; i++)
    {
        if (validator->checks[i].check_id == check_id)
        {
            *passed = validator->checks[i].passed_rules;
            *failed = validator->checks[i].failed_rules;
            return 0;
        }
    }

    return -1;
}

int system_validator_check_overall_health(SystemValidator *validator)
{
    if (!validator)
        return -1;

    uint32_t healthy = 0;
    for (uint32_t i = 0; i < validator->check_count; i++)
    {
        if (validator->checks[i].is_valid)
            healthy++;
    }

    return (healthy == validator->check_count) ? 0 : -1;
}

uint8_t system_validator_is_system_valid(SystemValidator *validator)
{
    if (!validator)
        return 0;

    for (uint32_t i = 0; i < validator->check_count; i++)
    {
        if (!validator->checks[i].is_valid)
            return 0;
    }

    return 1;
}

int validation_clear_errors(SystemValidator *validator, uint32_t check_id)
{
    if (!validator || check_id == 0)
        return -1;

    for (uint32_t i = 0; i < validator->check_count; i++)
    {
        if (validator->checks[i].check_id == check_id)
        {
            ComponentCheck *check = &validator->checks[i];
            check->error_count = 0;
            memset(check->errors, 0, sizeof(check->errors));
            return 0;
        }
    }

    return -1;
}

int validation_generate_report(SystemValidator *validator, const char *filepath)
{
    if (!validator || !filepath)
        return -1;

    return 0;
}

int system_validator_get_metrics(SystemValidator *validator, ValidatorMetrics *metrics)
{
    if (!validator || !metrics)
        return -1;

    metrics->validator_id = validator->validator_id;
    metrics->total_components = validator->check_count;
    metrics->passed_components = validator->components_passed;
    metrics->failed_components = validator->components_failed;

    uint32_t total_errors = 0;
    for (uint32_t i = 0; i < validator->check_count; i++)
    {
        total_errors += validator->checks[i].error_count;
    }

    metrics->total_errors = total_errors;
    metrics->system_valid = system_validator_is_system_valid(validator);

    return 0;
}
