#ifndef SENTRY_SETUP_H
#define SENTRY_SETUP_H

#include <stdint.h>

int sentry_setup_init(void);
int sentry_setup_configure_monitoring(const char *dsn);
int sentry_setup_enable_error_tracking(void);
int sentry_setup_enable_performance_monitoring(void);
int sentry_setup_set_environment(const char *environment);
int sentry_setup_test_integration(void);

#endif
