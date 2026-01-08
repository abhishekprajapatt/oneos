#ifndef UI_CORE_H
#define UI_CORE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t refresh_rate;
} Display;

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} Color;

typedef enum
{
    UI_LIGHT_MODE,
    UI_DARK_MODE,
    UI_AUTO_MODE
} UITheme;

typedef struct
{
    char name[256];
    UITheme theme;
    uint32_t accent_color;
    float animation_speed;
    bool reduced_motion;
} UIPreferences;

typedef struct
{
    Display primary_display;
    Display *secondary_displays;
    uint32_t display_count;
    UIPreferences preferences;
} UICore;

UICore *ui_core_init(void);
void ui_core_cleanup(UICore *core);
void ui_set_theme(UICore *core, UITheme theme);
void ui_draw_menu_bar(UICore *core);
void ui_draw_dock(UICore *core);
void ui_draw_spotlight_search(UICore *core);
int ui_update_frame(UICore *core);

#endif
