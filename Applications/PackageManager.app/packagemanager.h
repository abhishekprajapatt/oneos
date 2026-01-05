#ifndef PACKAGEMANAGER_H
#define PACKAGEMANAGER_H

#include <stdint.h>

typedef struct
{
    uint32_t app_id;
    const char *app_name;
    const char *app_version;
} packagemanager_config_t;

int packagemanager_init(void);
int packagemanager_load_package(const char *package_name);
int packagemanager_unload_package(const char *package_name);
int packagemanager_update_package(const char *package_name);
int packagemanager_list_packages(void);
int packagemanager_search_package(const char *query);
int packagemanager_validate_package(const char *package_name);

#endif
