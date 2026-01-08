#include "application_manager.h"
#include <stdlib.h>
#include <string.h>

ApplicationManager *app_manager_create(void)
{
    ApplicationManager *am = (ApplicationManager *)malloc(sizeof(ApplicationManager));
    if (!am)
        return NULL;

    am->app_count = 0;
    memset(am->apps, 0, sizeof(am->apps));

    return am;
}

void app_manager_destroy(ApplicationManager *am)
{
    if (!am)
        return;
    free(am);
}

uint32_t app_manager_install_app(ApplicationManager *am, const char *app_path, const char *name)
{
    if (!am || am->app_count >= MAX_APPS || !app_path || !name)
        return 0;

    Application *app = &am->apps[am->app_count];
    app->app_id = am->app_count + 1;
    strncpy(app->name, name, 255);
    strncpy(app->version, "1.0.0", 31);
    strncpy(app->executable_path, app_path, 511);
    app->state = APP_STATE_INSTALLED;
    app->pid = 0;
    app->requires_admin = false;
    app->launch_count = 0;
    app->category = APP_CATEGORY_UTILITIES;
    app->size = 0;

    am->app_count++;
    return app->app_id;
}

int app_manager_uninstall_app(ApplicationManager *am, uint32_t app_id)
{
    if (!am)
        return -1;

    for (uint32_t i = 0; i < am->app_count; i++)
    {
        if (am->apps[i].app_id == app_id)
        {
            for (uint32_t j = i; j < am->app_count - 1; j++)
            {
                am->apps[j] = am->apps[j + 1];
            }
            am->app_count--;
            return 0;
        }
    }

    return -1;
}

int app_manager_launch_app(ApplicationManager *am, uint32_t app_id)
{
    if (!am)
        return -1;

    Application *app = app_manager_get_app(am, app_id);
    if (!app)
        return -1;

    app->state = APP_STATE_RUNNING;
    app->pid = app_id * 100;
    app->launch_count++;

    return app->pid;
}

int app_manager_terminate_app(ApplicationManager *am, uint32_t app_id)
{
    if (!am)
        return -1;

    Application *app = app_manager_get_app(am, app_id);
    if (!app)
        return -1;

    app->state = APP_STATE_INSTALLED;
    app->pid = 0;

    return 0;
}

Application *app_manager_find_app(ApplicationManager *am, const char *name)
{
    if (!am || !name)
        return NULL;

    for (uint32_t i = 0; i < am->app_count; i++)
    {
        if (strcmp(am->apps[i].name, name) == 0)
        {
            return &am->apps[i];
        }
    }

    return NULL;
}

Application *app_manager_get_app(ApplicationManager *am, uint32_t app_id)
{
    if (!am)
        return NULL;

    for (uint32_t i = 0; i < am->app_count; i++)
    {
        if (am->apps[i].app_id == app_id)
        {
            return &am->apps[i];
        }
    }

    return NULL;
}

int app_manager_list_apps(ApplicationManager *am, AppCategory category, Application **apps)
{
    if (!am || !apps)
        return 0;

    uint32_t count = 0;
    for (uint32_t i = 0; i < am->app_count; i++)
    {
        if (am->apps[i].category == category)
        {
            count++;
        }
    }

    return count;
}

int app_manager_update_app(ApplicationManager *am, uint32_t app_id, const char *new_version)
{
    if (!am || !new_version)
        return -1;

    Application *app = app_manager_get_app(am, app_id);
    if (!app)
        return -1;

    strncpy(app->version, new_version, 31);

    return 0;
}
