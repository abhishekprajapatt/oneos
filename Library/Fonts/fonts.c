#include "fonts.h"

int fonts_init(void)
{
    return 0;
}

int fonts_load_font(const char *font_path)
{
    if (!font_path)
    {
        return -1;
    }
    return 0;
}

int fonts_list_fonts(font_info_t **fonts, uint32_t *count)
{
    if (!fonts || !count)
    {
        return -1;
    }
    *count = 0;
    return 0;
}

int fonts_get_font_info(const char *font_name, font_info_t *info)
{
    if (!font_name || !info)
    {
        return -1;
    }
    return 0;
}

int fonts_unload_font(const char *font_name)
{
    if (!font_name)
    {
        return -1;
    }
    return 0;
}

int fonts_set_default_font(const char *font_name)
{
    if (!font_name)
    {
        return -1;
    }
    return 0;
}
