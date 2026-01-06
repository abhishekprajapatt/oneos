#include "tests.h"

int tests_init(void)
{
    return 0;
}

int tests_register(const char *test_name, test_func_t test)
{
    if (!test_name || !test)
    {
        return -1;
    }
    return 0;
}

int tests_run_all(void)
{
    return 0;
}

int tests_run_single(const char *test_name)
{
    if (!test_name)
    {
        return -1;
    }
    return 0;
}

int tests_get_results(char **results, uint32_t max_results)
{
    if (!results || max_results == 0)
    {
        return -1;
    }
    return 0;
}
