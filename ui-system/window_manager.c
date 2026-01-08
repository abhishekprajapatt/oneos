#include "window_manager.h"
#include <stdlib.h>
#include <string.h>

WindowManager *wm_create(uint32_t screen_width, uint32_t screen_height)
{
    WindowManager *wm = (WindowManager *)malloc(sizeof(WindowManager));
    if (!wm)
        return NULL;

    wm->window_count = 0;
    wm->focused_window = 0;
    wm->screen_width = screen_width;
    wm->screen_height = screen_height;

    return wm;
}

void wm_destroy(WindowManager *wm)
{
    if (!wm)
        return;
    free(wm);
}

uint32_t wm_create_window(WindowManager *wm, const char *title, uint32_t x, uint32_t y,
                          uint32_t width, uint32_t height, uint32_t pid)
{
    if (!wm || wm->window_count >= MAX_WINDOWS || !title)
        return 0;

    Window *win = &wm->windows[wm->window_count];
    win->window_id = wm->window_count + 1;
    strcpy(win->title, title);
    win->x = x;
    win->y = y;
    win->width = width;
    win->height = height;
    win->state = WIN_NORMAL;
    win->is_focused = false;
    win->pid = pid;
    win->z_index = wm->window_count;

    wm->window_count++;
    return win->window_id;
}

void wm_destroy_window(WindowManager *wm, uint32_t window_id)
{
    if (!wm)
        return;

    for (uint32_t i = 0; i < wm->window_count; i++)
    {
        if (wm->windows[i].window_id == window_id)
        {
            for (uint32_t j = i; j < wm->window_count - 1; j++)
            {
                wm->windows[j] = wm->windows[j + 1];
            }
            wm->window_count--;
            break;
        }
    }
}

void wm_focus_window(WindowManager *wm, uint32_t window_id)
{
    if (!wm)
        return;

    for (uint32_t i = 0; i < wm->window_count; i++)
    {
        wm->windows[i].is_focused = (wm->windows[i].window_id == window_id);
    }
    wm->focused_window = window_id;
}

void wm_minimize_window(WindowManager *wm, uint32_t window_id)
{
    if (!wm)
        return;

    for (uint32_t i = 0; i < wm->window_count; i++)
    {
        if (wm->windows[i].window_id == window_id)
        {
            wm->windows[i].state = WIN_MINIMIZED;
            break;
        }
    }
}

void wm_maximize_window(WindowManager *wm, uint32_t window_id)
{
    if (!wm)
        return;

    for (uint32_t i = 0; i < wm->window_count; i++)
    {
        if (wm->windows[i].window_id == window_id)
        {
            wm->windows[i].state = WIN_MAXIMIZED;
            wm->windows[i].x = 0;
            wm->windows[i].y = 26;
            wm->windows[i].width = wm->screen_width;
            wm->windows[i].height = wm->screen_height - 26 - 78;
            break;
        }
    }
}

void wm_arrange_windows(WindowManager *wm)
{
    if (!wm)
        return;
}

void wm_render(WindowManager *wm)
{
    if (!wm)
        return;
}
