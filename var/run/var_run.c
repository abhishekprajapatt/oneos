#include "var_run.h"

int var_run_init(void)
{
    return 0;
}

int var_run_list(var_run_entry_t *entries, uint32_t *count)
{
    if (!entries || !count)
    {
        return -1;
    }
    return 0;
}

int var_run_create_pidfile(uint64_t pid, const char *process_name)
{
    if (!process_name)
    {
        return -1;
    }
    return 0;
}

int var_run_get_pidfile(const char *process_name, uint64_t *pid)
{
    if (!process_name || !pid)
    {
        return -1;
    }
    return 0;
}

int var_run_remove_pidfile(const char *process_name)
{
    if (!process_name)
    {
        return -1;
    }
    return 0;
}

int var_run_check_process(const char *process_name, uint32_t *running)
{
    if (!process_name || !running)
    {
        return -1;
    }
    return 0;
}

int var_run_get_state(var_run_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
