#include "sentry_setup.h"

int sentry_setup_init(void)
{
    return 0;
}

int sentry_setup_configure_monitoring(const char *dsn)
{
    if (!dsn)
    {
        return -1;
    }
    return 0;
}

int sentry_setup_enable_error_tracking(void)
{
    return 0;
}

int sentry_setup_enable_performance_monitoring(void)
{
    return 0;
}

int sentry_setup_set_environment(const char *environment)
{
    if (!environment)
    {
        return -1;
    }
    return 0;
}

int sentry_setup_test_integration(void)
{
    return 0;
}
