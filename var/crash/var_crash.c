#include "var_crash.h"

int var_crash_init(void)
{
    return 0;
}

int var_crash_list(var_crash_entry_t *crashes, uint32_t *count)
{
    if (!crashes || !count)
    {
        return -1;
    }
    return 0;
}

int var_crash_get_dump(const char *crash_name, var_crash_entry_t *crash)
{
    if (!crash_name || !crash)
    {
        return -1;
    }
    return 0;
}

int var_crash_analyze(const char *crash_name, char *report, uint32_t *report_size)
{
    if (!crash_name || !report || !report_size)
    {
        return -1;
    }
    return 0;
}

int var_crash_remove(const char *crash_name)
{
    if (!crash_name)
    {
        return -1;
    }
    return 0;
}

int var_crash_get_state(var_crash_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
