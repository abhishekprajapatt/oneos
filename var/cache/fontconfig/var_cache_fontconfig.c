#include "var_cache_fontconfig.h"

int var_cache_fontconfig_init(void)
{
    return 0;
}

int var_cache_fontconfig_list(var_cache_fontconfig_entry_t *entries, uint32_t *count)
{
    if (!entries || !count)
    {
        return -1;
    }
    return 0;
}

int var_cache_fontconfig_get(const char *font_name, var_cache_fontconfig_entry_t *entry)
{
    if (!font_name || !entry)
    {
        return -1;
    }
    return 0;
}

int var_cache_fontconfig_cache_font(const char *font_name, const char *font_path)
{
    if (!font_name || !font_path)
    {
        return -1;
    }
    return 0;
}

int var_cache_fontconfig_invalidate(const char *font_name)
{
    if (!font_name)
    {
        return -1;
    }
    return 0;
}

int var_cache_fontconfig_rebuild(void)
{
    return 0;
}

int var_cache_fontconfig_get_state(var_cache_fontconfig_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
