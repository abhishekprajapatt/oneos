#include "menu_bar.h"
#include <stdlib.h>
#include <string.h>

MenuBar *menubar_create(void)
{
    MenuBar *bar = (MenuBar *)malloc(sizeof(MenuBar));
    if (!bar)
        return NULL;

    bar->height = 26;
    bar->bg_color = 0x1E1E1E;
    bar->text_color = 0xFFFFFF;
    bar->item_count = 0;
    bar->items = NULL;

    return bar;
}

void menubar_destroy(MenuBar *bar)
{
    if (!bar)
        return;
    if (bar->items)
    {
        free(bar->items);
    }
    free(bar);
}

void menubar_render(MenuBar *bar)
{
    if (!bar)
        return;
}

void menubar_add_item(MenuBar *bar, const char *title, MenuType type)
{
    if (!bar || !title)
        return;

    MenuItem *new_items = (MenuItem *)realloc(bar->items,
                                              (bar->item_count + 1) * sizeof(MenuItem));
    if (!new_items)
        return;

    bar->items = new_items;
    strcpy(bar->items[bar->item_count].title, title);
    bar->items[bar->item_count].type = type;
    bar->items[bar->item_count].is_open = false;
    bar->item_count++;
}

void menubar_toggle_menu(MenuBar *bar, MenuType type)
{
    if (!bar)
        return;

    for (uint32_t i = 0; i < bar->item_count; i++)
    {
        if (bar->items[i].type == type)
        {
            bar->items[i].is_open = !bar->items[i].is_open;
        }
        else
        {
            bar->items[i].is_open = false;
        }
    }
}
