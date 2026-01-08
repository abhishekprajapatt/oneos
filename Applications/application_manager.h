#ifndef APPLICATION_MANAGER_H
#define APPLICATION_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_APPS 1000

typedef enum
{
    APP_STATE_NOT_INSTALLED,
    APP_STATE_INSTALLED,
    APP_STATE_RUNNING,
    APP_STATE_PAUSED,
    APP_STATE_SUSPENDED
} AppState;

typedef enum
{
    APP_CATEGORY_SYSTEM,
    APP_CATEGORY_PRODUCTIVITY,
    APP_CATEGORY_MULTIMEDIA,
    APP_CATEGORY_DEVELOPMENT,
    APP_CATEGORY_GAMES,
    APP_CATEGORY_UTILITIES,
    APP_CATEGORY_NETWORK
} AppCategory;

typedef struct
{
    uint32_t app_id;
    char name[256];
    char version[32];
    char developer[256];
    char description[512];
    AppCategory category;
    uint64_t size;
    AppState state;
    uint32_t pid;
    bool requires_admin;
    uint32_t launch_count;
    char executable_path[512];
} Application;

typedef struct
{
    Application apps[MAX_APPS];
    uint32_t app_count;
} ApplicationManager;

ApplicationManager *app_manager_create(void);
void app_manager_destroy(ApplicationManager *am);
uint32_t app_manager_install_app(ApplicationManager *am, const char *app_path, const char *name);
int app_manager_uninstall_app(ApplicationManager *am, uint32_t app_id);
int app_manager_launch_app(ApplicationManager *am, uint32_t app_id);
int app_manager_terminate_app(ApplicationManager *am, uint32_t app_id);
Application *app_manager_find_app(ApplicationManager *am, const char *name);
Application *app_manager_get_app(ApplicationManager *am, uint32_t app_id);
int app_manager_list_apps(ApplicationManager *am, AppCategory category, Application **apps);
int app_manager_update_app(ApplicationManager *am, uint32_t app_id, const char *new_version);

#endif
