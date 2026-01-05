#include "var_spool_cron.h"

int var_spool_cron_init(void)
{
    return 0;
}

int var_spool_cron_list(var_spool_cron_entry_t *jobs, uint32_t *count)
{
    if (!jobs || !count)
    {
        return -1;
    }
    return 0;
}

int var_spool_cron_get_user_jobs(const char *user, var_spool_cron_entry_t *jobs, uint32_t *count)
{
    if (!user || !jobs || !count)
    {
        return -1;
    }
    return 0;
}

int var_spool_cron_add_job(const char *user, const char *job, const char *schedule)
{
    if (!user || !job || !schedule)
    {
        return -1;
    }
    return 0;
}

int var_spool_cron_remove_job(const char *user, const char *job)
{
    if (!user || !job)
    {
        return -1;
    }
    return 0;
}

int var_spool_cron_get_state(var_spool_cron_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
