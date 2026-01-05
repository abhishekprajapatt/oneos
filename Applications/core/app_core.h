#ifndef APP_CORE_H
#define APP_CORE_H

#include <stdint.h>

typedef struct
{
    uint32_t app_id;
    const char *app_name;
    const char *app_version;
    uint32_t is_running;
    uint32_t process_id;
} app_instance_t;

typedef struct
{
    uint32_t total_apps;
    uint32_t running_apps;
    uint32_t stopped_apps;
    uint64_t total_memory;
} app_core_stats_t;

int app_core_init(void);
int app_core_launch(const char *app_name, uint32_t *app_id);
int app_core_terminate(uint32_t app_id);
int app_core_get_app_info(uint32_t app_id, app_instance_t *info);
int app_core_list_running_apps(app_instance_t *apps, uint32_t *count);
int app_core_suspend_app(uint32_t app_id);
int app_core_resume_app(uint32_t app_id);
int app_core_get_stats(app_core_stats_t *stats);

#endif
