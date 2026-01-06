#include "sys_fonts.h"

int sys_fonts_init(void)
{
    return 0;
}

int sys_fonts_register(sys_font_t *font)
{
    if (!font || !font->family)
    {
        return -1;
    }
    return 0;
}

int sys_fonts_unregister(const char *font_name)
{
    if (!font_name)
    {
        return -1;
    }
    return 0;
}

int sys_fonts_list(sys_font_t **fonts, uint32_t *count)
{
    if (!fonts || !count)
    {
        return -1;
    }
    *count = 0;
    return 0;
}

int sys_fonts_get_default(sys_font_t *font)
{
    if (!font)
    {
        return -1;
    }
    return 0;
}

int sys_fonts_set_default(const char *font_name)
{
    if (!font_name)
    {
        return -1;
    }
    return 0;
}
