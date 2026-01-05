#include "system_preferences.h"

int system_preferences_init(void)
{
    return 0;
}

int system_preferences_set(system_preferences_entry_t *preference)
{
    if (!preference)
    {
        return -1;
    }
    return 0;
}

int system_preferences_get(const char *pref_key, system_preferences_entry_t *preference)
{
    if (!pref_key || !preference)
    {
        return -1;
    }
    return 0;
}

int system_preferences_delete(uint32_t pref_id)
{
    return 0;
}

int system_preferences_reset_defaults(void)
{
    return 0;
}

int system_preferences_save(void)
{
    return 0;
}

int system_preferences_get_state(system_preferences_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
