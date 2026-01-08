#ifndef DOCK_H
#define DOCK_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_DOCK_APPS 50

typedef enum
{
    APP_FINDER,
    APP_SYSTEM_PREFERENCES,
    APP_TERMINAL,
    APP_BROWSER,
    APP_FILE_MANAGER,
    APP_CUSTOM
} AppType;

typedef struct
{
    uint32_t app_id;
    char name[128];
    char icon_path[256];
    AppType type;
    uint32_t x_pos;
    uint32_t y_pos;
    uint32_t width;
    uint32_t height;
    bool is_running;
    uint32_t pid;
} DockApp;

typedef struct
{
    DockApp apps[MAX_DOCK_APPS];
    uint32_t app_count;
    uint32_t position_y;
    uint32_t height;
    uint32_t width;
    uint32_t bg_color;
    bool show_labels;
    float zoom_level;
} Dock;

Dock *dock_create(void);
void dock_destroy(Dock *dock);
void dock_render(Dock *dock);
void dock_add_app(Dock *dock, const char *name, AppType type, const char *icon_path);
void dock_launch_app(Dock *dock, uint32_t app_id);
void dock_update_positions(Dock *dock);

#endif
