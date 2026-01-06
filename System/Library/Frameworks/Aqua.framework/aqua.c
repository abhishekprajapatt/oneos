#include "aqua.h"

int aqua_init(void)
{
    return 0;
}

int aqua_render_theme(const char *theme_name)
{
    if (!theme_name)
    {
        return -1;
    }
    return 0;
}

int aqua_set_appearance(uint32_t appearance)
{
    return 0;
}

int aqua_get_appearance(uint32_t *appearance)
{
    if (!appearance)
    {
        return -1;
    }
    *appearance = 0;
    return 0;
}

int aqua_apply_effects(void *surface, uint32_t effect_type)
{
    if (!surface)
    {
        return -1;
    }
    return 0;
}
