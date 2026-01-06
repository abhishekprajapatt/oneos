#include "appkit.h"

int appkit_init(void)
{
    return 0;
}

int appkit_create_application(NSApplication **app)
{
    if (!app)
    {
        return -1;
    }
    return 0;
}

int appkit_create_window(uint32_t width, uint32_t height, NSWindow **window)
{
    if (width == 0 || height == 0 || !window)
    {
        return -1;
    }
    return 0;
}

int appkit_run_event_loop(void)
{
    return 0;
}

int appkit_shutdown(void)
{
    return 0;
}

int appkit_set_menu(const char *menu_items)
{
    if (!menu_items)
    {
        return -1;
    }
    return 0;
}
