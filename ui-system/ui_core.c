#include "ui_core.h"
#include <stdio.h>
#include <string.h>

UICore *ui_core_init(void)
{
    UICore *core = (UICore *)malloc(sizeof(UICore));
    if (!core)
        return NULL;

    core->primary_display.width = 1920;
    core->primary_display.height = 1080;
    core->primary_display.depth = 32;
    core->primary_display.refresh_rate = 60;

    core->display_count = 1;
    core->secondary_displays = NULL;

    strcpy(core->preferences.name, "OneOS");
    core->preferences.theme = UI_DARK_MODE;
    core->preferences.accent_color = 0x007AFF;
    core->preferences.animation_speed = 1.0f;
    core->preferences.reduced_motion = false;

    return core;
}

void ui_core_cleanup(UICore *core)
{
    if (!core)
        return;
    if (core->secondary_displays)
    {
        free(core->secondary_displays);
    }
    free(core);
}

void ui_set_theme(UICore *core, UITheme theme)
{
    if (!core)
        return;
    core->preferences.theme = theme;
}

void ui_draw_menu_bar(UICore *core)
{
    if (!core)
        return;
}

void ui_draw_dock(UICore *core)
{
    if (!core)
        return;
}

void ui_draw_spotlight_search(UICore *core)
{
    if (!core)
        return;
}

int ui_update_frame(UICore *core)
{
    if (!core)
        return -1;
    return 0;
}
