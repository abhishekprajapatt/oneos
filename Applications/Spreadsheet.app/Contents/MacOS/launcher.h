#ifndef APP_LAUNCHER_H
#define APP_LAUNCHER_H

#include <stdint.h>

typedef struct
{
    const char *executable_path;
    const char *app_name;
    const char *app_version;
    uint32_t minimum_os_version;
} launcher_config_t;

int launcher_init(const char *app_path);
int launcher_load_config(launcher_config_t *config);
int launcher_start_app(void);
int launcher_validate_environment(void);
int launcher_set_working_directory(const char *directory);
int launcher_load_resources(void);
int launcher_cleanup(void);

#endif
