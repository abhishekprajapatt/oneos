#ifndef TESTS_H
#define TESTS_H

#include <stdint.h>

typedef int (*test_func_t)(void);

int tests_init(void);
int tests_register(const char *test_name, test_func_t test);
int tests_run_all(void);
int tests_run_single(const char *test_name);
int tests_get_results(char **results, uint32_t max_results);

#endif
