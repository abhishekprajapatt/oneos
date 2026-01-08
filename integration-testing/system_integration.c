#include "system_integration.h"
#include <stdlib.h>
#include <string.h>

static TestFramework *global_framework = NULL;
static uint32_t next_test_id = 1;

TestFramework *test_framework_init(void)
{
    TestFramework *framework = (TestFramework *)malloc(sizeof(TestFramework));
    if (!framework)
        return NULL;

    memset(framework, 0, sizeof(TestFramework));
    framework->framework_id = 1;
    framework->test_count = 0;
    framework->passed_tests = 0;
    framework->failed_tests = 0;
    framework->skipped_tests = 0;
    framework->total_execution_time_ms = 0;

    global_framework = framework;
    return framework;
}

int test_framework_destroy(TestFramework *framework)
{
    if (!framework)
        return -1;

    free(framework);
    global_framework = NULL;
    return 0;
}

uint32_t integration_test_create(TestFramework *framework, const char *test_name,
                                 const char *description)
{
    if (!framework || framework->test_count >= MAX_INTEGRATION_TESTS || !test_name)
        return 0;

    IntegrationTest *test = &framework->tests[framework->test_count];
    memset(test, 0, sizeof(IntegrationTest));

    test->test_id = next_test_id++;
    strncpy(test->test_name, test_name, sizeof(test->test_name) - 1);
    if (description)
    {
        strncpy(test->test_description, description, sizeof(test->test_description) - 1);
    }

    test->step_count = 0;
    test->assertion_count = 0;
    test->dependency_count = 0;
    test->status = TEST_STATUS_NOT_RUN;
    test->execution_time_ms = 0;

    framework->test_count++;

    return test->test_id;
}

int integration_test_destroy(TestFramework *framework, uint32_t test_id)
{
    if (!framework || test_id == 0)
        return -1;

    for (uint32_t i = 0; i < framework->test_count; i++)
    {
        if (framework->tests[i].test_id == test_id)
        {
            memmove(&framework->tests[i], &framework->tests[i + 1],
                    sizeof(IntegrationTest) * (framework->test_count - i - 1));
            framework->test_count--;
            return 0;
        }
    }

    return -1;
}

int test_add_step(TestFramework *framework, uint32_t test_id, const char *step_name,
                  const char *description, uint8_t is_critical, uint32_t timeout_ms)
{
    if (!framework || test_id == 0 || !step_name)
        return -1;

    for (uint32_t i = 0; i < framework->test_count; i++)
    {
        if (framework->tests[i].test_id == test_id)
        {
            IntegrationTest *test = &framework->tests[i];

            if (test->step_count >= MAX_TEST_STEPS)
                return -1;

            TestStep *step = &test->steps[test->step_count];
            memset(step, 0, sizeof(TestStep));

            step->step_id = test->step_count + 1;
            strncpy(step->step_name, step_name, sizeof(step->step_name) - 1);
            if (description)
            {
                strncpy(step->step_description, description, sizeof(step->step_description) - 1);
            }

            step->is_critical = is_critical;
            step->timeout_ms = timeout_ms;
            step->is_async = 0;

            test->step_count++;
            return 0;
        }
    }

    return -1;
}

int test_add_dependency(TestFramework *framework, uint32_t test_id, uint32_t dependency_test_id)
{
    if (!framework || test_id == 0 || dependency_test_id == 0)
        return -1;

    for (uint32_t i = 0; i < framework->test_count; i++)
    {
        if (framework->tests[i].test_id == test_id)
        {
            IntegrationTest *test = &framework->tests[i];

            if (test->dependency_count >= MAX_TEST_DEPENDENCIES)
                return -1;

            test->dependencies[test->dependency_count] = dependency_test_id;
            test->dependency_count++;
            return 0;
        }
    }

    return -1;
}

int test_add_assertion(TestFramework *framework, uint32_t test_id, AssertionType type,
                       int64_t expected, int64_t actual, const char *message)
{
    if (!framework || test_id == 0 || !message)
        return -1;

    for (uint32_t i = 0; i < framework->test_count; i++)
    {
        if (framework->tests[i].test_id == test_id)
        {
            IntegrationTest *test = &framework->tests[i];

            if (test->assertion_count >= MAX_TEST_ASSERTIONS)
                return -1;

            Assertion *assertion = &test->assertions[test->assertion_count];
            memset(assertion, 0, sizeof(Assertion));

            assertion->assertion_id = test->assertion_count + 1;
            assertion->type = type;
            assertion->expected_value = expected;
            assertion->actual_value = actual;
            strncpy(assertion->assertion_message, message, sizeof(assertion->assertion_message) - 1);

            assertion->passed = (expected == actual) ? 1 : 0;

            test->assertion_count++;
            return 0;
        }
    }

    return -1;
}

int integration_test_run(TestFramework *framework, uint32_t test_id)
{
    if (!framework || test_id == 0)
        return -1;

    for (uint32_t i = 0; i < framework->test_count; i++)
    {
        if (framework->tests[i].test_id == test_id)
        {
            IntegrationTest *test = &framework->tests[i];

            test->status = TEST_STATUS_RUNNING;
            test->execution_time_ms = 0;

            uint32_t passed_assertions = 0;
            for (uint32_t j = 0; j < test->assertion_count; j++)
            {
                if (test->assertions[j].passed)
                    passed_assertions++;
            }

            if (passed_assertions == test->assertion_count)
            {
                test->status = TEST_STATUS_PASSED;
                framework->passed_tests++;
            }
            else
            {
                test->status = TEST_STATUS_FAILED;
                framework->failed_tests++;
            }

            return 0;
        }
    }

    return -1;
}

int integration_test_run_all(TestFramework *framework)
{
    if (!framework)
        return -1;

    for (uint32_t i = 0; i < framework->test_count; i++)
    {
        integration_test_run(framework, framework->tests[i].test_id);
    }

    return 0;
}

int test_check_dependencies(TestFramework *framework, uint32_t test_id)
{
    if (!framework || test_id == 0)
        return -1;

    for (uint32_t i = 0; i < framework->test_count; i++)
    {
        if (framework->tests[i].test_id == test_id)
        {
            IntegrationTest *test = &framework->tests[i];

            for (uint32_t j = 0; j < test->dependency_count; j++)
            {
                for (uint32_t k = 0; k < framework->test_count; k++)
                {
                    if (framework->tests[k].test_id == test->dependencies[j])
                    {
                        if (framework->tests[k].status != TEST_STATUS_PASSED)
                        {
                            test->status = TEST_STATUS_SKIPPED;
                            framework->skipped_tests++;
                            return -1;
                        }
                    }
                }
            }

            return 0;
        }
    }

    return -1;
}

TestStatus integration_test_get_status(TestFramework *framework, uint32_t test_id)
{
    if (!framework || test_id == 0)
        return TEST_STATUS_NOT_RUN;

    for (uint32_t i = 0; i < framework->test_count; i++)
    {
        if (framework->tests[i].test_id == test_id)
        {
            return framework->tests[i].status;
        }
    }

    return TEST_STATUS_NOT_RUN;
}

uint32_t test_get_passed_count(TestFramework *framework)
{
    if (!framework)
        return 0;

    return framework->passed_tests;
}

uint32_t test_get_failed_count(TestFramework *framework)
{
    if (!framework)
        return 0;

    return framework->failed_tests;
}

uint32_t test_get_skipped_count(TestFramework *framework)
{
    if (!framework)
        return 0;

    return framework->skipped_tests;
}

int test_framework_generate_report(TestFramework *framework, const char *filepath)
{
    if (!framework || !filepath)
        return -1;

    return 0;
}

int test_framework_clear_results(TestFramework *framework)
{
    if (!framework)
        return -1;

    framework->passed_tests = 0;
    framework->failed_tests = 0;
    framework->skipped_tests = 0;
    framework->total_execution_time_ms = 0;

    for (uint32_t i = 0; i < framework->test_count; i++)
    {
        framework->tests[i].status = TEST_STATUS_NOT_RUN;
        framework->tests[i].execution_time_ms = 0;
    }

    return 0;
}

int test_framework_get_metrics(TestFramework *framework, TestFrameworkMetrics *metrics)
{
    if (!framework || !metrics)
        return -1;

    metrics->framework_id = framework->framework_id;
    metrics->total_tests = framework->test_count;
    metrics->passed_tests = framework->passed_tests;
    metrics->failed_tests = framework->failed_tests;
    metrics->skipped_tests = framework->skipped_tests;
    metrics->total_time_ms = framework->total_execution_time_ms;

    if (framework->test_count > 0)
    {
        metrics->pass_rate = ((double)framework->passed_tests / (double)framework->test_count) * 100.0;
    }
    else
    {
        metrics->pass_rate = 0.0;
    }

    return 0;
}
