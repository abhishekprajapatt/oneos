#include "var_run_lock.h"

int var_run_lock_init(void)
{
    return 0;
}

int var_run_lock_list(var_run_lock_entry_t *locks, uint32_t *count)
{
    if (!locks || !count)
    {
        return -1;
    }
    return 0;
}

int var_run_lock_acquire(const char *lock_name, uint64_t owner_pid)
{
    if (!lock_name)
    {
        return -1;
    }
    return 0;
}

int var_run_lock_release(const char *lock_name)
{
    if (!lock_name)
    {
        return -1;
    }
    return 0;
}

int var_run_lock_is_locked(const char *lock_name, uint32_t *locked)
{
    if (!lock_name || !locked)
    {
        return -1;
    }
    return 0;
}

int var_run_lock_cleanup_stale(void)
{
    return 0;
}

int var_run_lock_get_state(var_run_lock_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
