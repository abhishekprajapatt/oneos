#include "library_fonts.h"

int library_fonts_init(void)
{
    return 0;
}

int library_fonts_register(library_fonts_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int library_fonts_unregister(uint32_t font_id)
{
    return 0;
}

int library_fonts_get_font(const char *name, library_fonts_entry_t *entry)
{
    if (!name || !entry)
    {
        return -1;
    }
    return 0;
}

int library_fonts_list_family(const char *family, library_fonts_entry_t *results, uint32_t *count)
{
    if (!family || !results || !count)
    {
        return -1;
    }
    return 0;
}

int library_fonts_get_state(library_fonts_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
