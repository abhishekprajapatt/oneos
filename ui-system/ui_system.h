#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H

#include <stdint.h>

typedef struct
{
    uint32_t width;
    uint32_t height;
    uint32_t bits_per_pixel;
} ui_display_t;

int ui_init(void);
int ui_create_window(uint32_t width, uint32_t height, void **window);
int ui_destroy_window(void *window);
int ui_draw_pixel(void *window, uint32_t x, uint32_t y, uint32_t color);
int ui_render_text(void *window, uint32_t x, uint32_t y, const char *text);
int ui_get_display_info(ui_display_t *display);
int ui_handle_input(void **input_event);

#endif
