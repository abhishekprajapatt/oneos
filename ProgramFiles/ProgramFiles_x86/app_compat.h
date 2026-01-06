#ifndef APP_COMPAT_H
#define APP_COMPAT_H

#include <stdint.h>

typedef struct
{
    const char *app_name;
    uint32_t os_version;
    uint32_t required_features;
} app_compat_mode_t;

int app_compat_init(void);
int app_compat_enable_mode(const char *app_name, app_compat_mode_t *mode);
int app_compat_set_privileges(const char *app_name, uint32_t privilege_level);
int app_compat_disable_fullscreen_optimization(const char *app_name);
int app_compat_get_config(const char *app_name, char *config, uint32_t max_len);
int app_compat_test_launch(const char *app_name);

#endif
