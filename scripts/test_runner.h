#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <stdint.h>

typedef struct
{
    const char *test_name;
    int (*test_func)(void);
    uint32_t status;
} test_case_t;

int test_runner_init(void);
int test_runner_register(test_case_t *test);
int test_runner_run_all(void);
int test_runner_run_single(const char *test_name);
int test_runner_get_results(char **results, uint32_t max_results);
int test_runner_cleanup(void);

#endif
