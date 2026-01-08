#ifndef SYSTEM_VALIDATOR_H
#define SYSTEM_VALIDATOR_H

#include <stdint.h>
#include <stddef.h>

#define MAX_VALIDATION_RULES 128
#define MAX_VALIDATION_ERRORS 512
#define MAX_COMPONENT_CHECKS 256

typedef enum
{
    VALIDATION_RULE_FUNCTIONAL,
    VALIDATION_RULE_PERFORMANCE,
    VALIDATION_RULE_RELIABILITY,
    VALIDATION_RULE_SECURITY,
    VALIDATION_RULE_COMPATIBILITY
} ValidationRuleType;

typedef enum
{
    ERROR_LEVEL_INFO,
    ERROR_LEVEL_WARNING,
    ERROR_LEVEL_ERROR,
    ERROR_LEVEL_CRITICAL
} ValidationErrorLevel;

typedef struct
{
    uint32_t rule_id;
    char rule_name[256];
    ValidationRuleType rule_type;

    char rule_description[512];
    uint8_t is_critical;
    uint8_t is_enabled;
} ValidationRule;

typedef struct
{
    uint32_t error_id;
    ValidationErrorLevel level;
    char error_message[512];

    uint32_t component_id;
    uint64_t timestamp;
} ValidationError;

typedef struct
{
    uint32_t check_id;
    char component_name[256];

    ValidationRule rules[MAX_VALIDATION_RULES];
    uint32_t rule_count;

    ValidationError errors[MAX_VALIDATION_ERRORS];
    uint32_t error_count;

    uint32_t passed_rules;
    uint32_t failed_rules;
    uint8_t is_valid;
} ComponentCheck;

typedef struct
{
    uint32_t validator_id;

    ComponentCheck checks[MAX_COMPONENT_CHECKS];
    uint32_t check_count;

    uint32_t components_passed;
    uint32_t components_failed;
    uint64_t validation_start_time;
    uint64_t validation_end_time;
} SystemValidator;

SystemValidator *system_validator_init(void);
int system_validator_destroy(SystemValidator *validator);

uint32_t component_check_create(SystemValidator *validator, const char *component_name);

int component_check_destroy(SystemValidator *validator, uint32_t check_id);

uint32_t validation_rule_create(SystemValidator *validator, uint32_t check_id,
                                const char *rule_name, ValidationRuleType type);

int validation_rule_enable(SystemValidator *validator, uint32_t check_id, uint32_t rule_id);

int validation_rule_disable(SystemValidator *validator, uint32_t check_id, uint32_t rule_id);

int validation_rule_mark_critical(SystemValidator *validator, uint32_t check_id,
                                  uint32_t rule_id);

int component_validation_run(SystemValidator *validator, uint32_t check_id);

int component_validation_run_all(SystemValidator *validator);

int validation_add_error(SystemValidator *validator, uint32_t check_id,
                         ValidationErrorLevel level, const char *error_message,
                         uint32_t component_id);

uint32_t validation_get_error_count(SystemValidator *validator, uint32_t check_id);

uint32_t validation_get_error_count_by_level(SystemValidator *validator, uint32_t check_id,
                                             ValidationErrorLevel level);

int validation_get_results(SystemValidator *validator, uint32_t check_id,
                           uint32_t *passed, uint32_t *failed);

int system_validator_check_overall_health(SystemValidator *validator);

uint8_t system_validator_is_system_valid(SystemValidator *validator);

int validation_clear_errors(SystemValidator *validator, uint32_t check_id);

int validation_generate_report(SystemValidator *validator, const char *filepath);

typedef struct
{
    uint32_t validator_id;
    uint32_t total_components;
    uint32_t passed_components;
    uint32_t failed_components;
    uint32_t total_errors;
    uint8_t system_valid;
} ValidatorMetrics;

int system_validator_get_metrics(SystemValidator *validator, ValidatorMetrics *metrics);

#endif
