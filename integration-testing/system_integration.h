#ifndef SYSTEM_INTEGRATION_H
#define SYSTEM_INTEGRATION_H

#include <stdint.h>
#include <stddef.h>

#define MAX_INTEGRATION_TESTS 256
#define MAX_TEST_STEPS 512
#define MAX_TEST_DEPENDENCIES 64
#define MAX_TEST_ASSERTIONS 1024

typedef enum
{
    TEST_STATUS_NOT_RUN,
    TEST_STATUS_RUNNING,
    TEST_STATUS_PASSED,
    TEST_STATUS_FAILED,
    TEST_STATUS_SKIPPED
} TestStatus;

typedef enum
{
    ASSERTION_EQUAL,
    ASSERTION_NOT_EQUAL,
    ASSERTION_GREATER_THAN,
    ASSERTION_LESS_THAN,
    ASSERTION_TRUE,
    ASSERTION_FALSE
} AssertionType;

typedef struct
{
    uint32_t step_id;
    char step_name[256];
    char step_description[512];

    uint8_t is_critical;
    uint32_t timeout_ms;
    uint8_t is_async;
} TestStep;

typedef struct
{
    uint32_t assertion_id;
    AssertionType type;

    int64_t expected_value;
    int64_t actual_value;
    char assertion_message[512];

    uint8_t passed;
} Assertion;

typedef struct
{
    uint32_t test_id;
    char test_name[256];
    char test_description[512];

    TestStep steps[MAX_TEST_STEPS];
    uint32_t step_count;

    Assertion assertions[MAX_TEST_ASSERTIONS];
    uint32_t assertion_count;

    uint32_t dependencies[MAX_TEST_DEPENDENCIES];
    uint32_t dependency_count;

    TestStatus status;
    uint64_t execution_time_ms;
} IntegrationTest;

typedef struct
{
    uint32_t framework_id;

    IntegrationTest tests[MAX_INTEGRATION_TESTS];
    uint32_t test_count;

    uint32_t passed_tests;
    uint32_t failed_tests;
    uint32_t skipped_tests;

    uint64_t total_execution_time_ms;
} TestFramework;

TestFramework *test_framework_init(void);
int test_framework_destroy(TestFramework *framework);

uint32_t integration_test_create(TestFramework *framework, const char *test_name,
                                 const char *description);

int integration_test_destroy(TestFramework *framework, uint32_t test_id);

int test_add_step(TestFramework *framework, uint32_t test_id, const char *step_name,
                  const char *description, uint8_t is_critical, uint32_t timeout_ms);

int test_add_dependency(TestFramework *framework, uint32_t test_id, uint32_t dependency_test_id);

int test_add_assertion(TestFramework *framework, uint32_t test_id, AssertionType type,
                       int64_t expected, int64_t actual, const char *message);

int integration_test_run(TestFramework *framework, uint32_t test_id);

int integration_test_run_all(TestFramework *framework);

int test_check_dependencies(TestFramework *framework, uint32_t test_id);

TestStatus integration_test_get_status(TestFramework *framework, uint32_t test_id);

uint32_t test_get_passed_count(TestFramework *framework);

uint32_t test_get_failed_count(TestFramework *framework);

uint32_t test_get_skipped_count(TestFramework *framework);

int test_framework_generate_report(TestFramework *framework, const char *filepath);

int test_framework_clear_results(TestFramework *framework);

typedef struct
{
    uint32_t framework_id;
    uint32_t total_tests;
    uint32_t passed_tests;
    uint32_t failed_tests;
    uint32_t skipped_tests;
    double pass_rate;
    uint64_t total_time_ms;
} TestFrameworkMetrics;

int test_framework_get_metrics(TestFramework *framework, TestFrameworkMetrics *metrics);

#endif
