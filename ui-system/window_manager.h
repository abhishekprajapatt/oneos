#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_WINDOWS 256

typedef enum
{
    WIN_NORMAL,
    WIN_FULLSCREEN,
    WIN_MINIMIZED,
    WIN_MAXIMIZED
} WindowState;

typedef struct
{
    uint32_t window_id;
    char title[256];
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    WindowState state;
    bool is_focused;
    uint32_t pid;
    uint32_t z_index;
} Window;

typedef struct
{
    Window windows[MAX_WINDOWS];
    uint32_t window_count;
    uint32_t focused_window;
    uint32_t screen_width;
    uint32_t screen_height;
} WindowManager;

WindowManager *wm_create(uint32_t screen_width, uint32_t screen_height);
void wm_destroy(WindowManager *wm);
uint32_t wm_create_window(WindowManager *wm, const char *title, uint32_t x, uint32_t y,
                          uint32_t width, uint32_t height, uint32_t pid);
void wm_destroy_window(WindowManager *wm, uint32_t window_id);
void wm_focus_window(WindowManager *wm, uint32_t window_id);
void wm_minimize_window(WindowManager *wm, uint32_t window_id);
void wm_maximize_window(WindowManager *wm, uint32_t window_id);
void wm_arrange_windows(WindowManager *wm);
void wm_render(WindowManager *wm);

#endif
