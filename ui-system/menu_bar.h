#ifndef MENU_BAR_H
#define MENU_BAR_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    MENU_ONEOS,
    MENU_FILE,
    MENU_EDIT,
    MENU_VIEW,
    MENU_WINDOW,
    MENU_HELP
} MenuType;

typedef struct
{
    char title[128];
    MenuType type;
    uint32_t x_pos;
    uint32_t y_pos;
    uint32_t width;
    uint32_t height;
    bool is_open;
} MenuItem;

typedef struct
{
    MenuItem *items;
    uint32_t item_count;
    uint32_t height;
    uint32_t bg_color;
    uint32_t text_color;
} MenuBar;

MenuBar *menubar_create(void);
void menubar_destroy(MenuBar *bar);
void menubar_render(MenuBar *bar);
void menubar_add_item(MenuBar *bar, const char *title, MenuType type);
void menubar_toggle_menu(MenuBar *bar, MenuType type);

#endif
