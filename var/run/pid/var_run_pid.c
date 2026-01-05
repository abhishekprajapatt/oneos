#include "var_run_pid.h"

int var_run_pid_init(void)
{
    return 0;
}

int var_run_pid_list(var_run_pid_entry_t *pids, uint32_t *count)
{
    if (!pids || !count)
    {
        return -1;
    }
    return 0;
}

int var_run_pid_create(const char *process_name, uint64_t process_id)
{
    if (!process_name)
    {
        return -1;
    }
    return 0;
}

int var_run_pid_get(const char *process_name, var_run_pid_entry_t *pid)
{
    if (!process_name || !pid)
    {
        return -1;
    }
    return 0;
}

int var_run_pid_remove(const char *process_name)
{
    if (!process_name)
    {
        return -1;
    }
    return 0;
}

int var_run_pid_verify(const char *process_name, uint32_t *exists)
{
    if (!process_name || !exists)
    {
        return -1;
    }
    return 0;
}

int var_run_pid_get_state(var_run_pid_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
