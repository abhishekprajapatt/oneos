#include "dock.h"
#include <stdlib.h>
#include <string.h>

Dock *dock_create(void)
{
    Dock *dock = (Dock *)malloc(sizeof(Dock));
    if (!dock)
        return NULL;

    dock->app_count = 0;
    dock->position_y = 0;
    dock->height = 78;
    dock->width = 512;
    dock->bg_color = 0x222222;
    dock->show_labels = false;
    dock->zoom_level = 1.0f;

    return dock;
}

void dock_destroy(Dock *dock)
{
    if (!dock)
        return;
    free(dock);
}

void dock_render(Dock *dock)
{
    if (!dock)
        return;
}

void dock_add_app(Dock *dock, const char *name, AppType type, const char *icon_path)
{
    if (!dock || dock->app_count >= MAX_DOCK_APPS || !name)
        return;

    DockApp *app = &dock->apps[dock->app_count];
    strcpy(app->name, name);
    app->type = type;
    strcpy(app->icon_path, icon_path);
    app->is_running = false;
    app->pid = 0;
    app->app_id = dock->app_count;

    dock->app_count++;
    dock_update_positions(dock);
}

void dock_launch_app(Dock *dock, uint32_t app_id)
{
    if (!dock || app_id >= dock->app_count)
        return;

    dock->apps[app_id].is_running = true;
}

void dock_update_positions(Dock *dock)
{
    if (!dock)
        return;

    uint32_t app_width = 50;
    uint32_t spacing = 10;
    uint32_t start_x = 10;

    for (uint32_t i = 0; i < dock->app_count; i++)
    {
        dock->apps[i].x_pos = start_x + i * (app_width + spacing);
        dock->apps[i].y_pos = dock->position_y;
        dock->apps[i].width = app_width;
        dock->apps[i].height = app_width;
    }
}
