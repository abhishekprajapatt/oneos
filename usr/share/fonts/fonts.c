#include "fonts.h"

int fonts_init(void)
{
    return 0;
}

int fonts_list_fonts(font_entry_t *fonts, uint32_t *count)
{
    if (!fonts || !count)
    {
        return -1;
    }
    return 0;
}

int fonts_get_font(const char *font_name, font_entry_t *font)
{
    if (!font_name || !font)
    {
        return -1;
    }
    return 0;
}

int fonts_search_by_family(const char *family, font_entry_t *fonts, uint32_t *count)
{
    if (!family || !fonts || !count)
    {
        return -1;
    }
    return 0;
}

int fonts_install_font(const char *font_path)
{
    if (!font_path)
    {
        return -1;
    }
    return 0;
}

int fonts_get_stats(font_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
