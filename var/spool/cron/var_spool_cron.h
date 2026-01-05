#ifndef VAR_SPOOL_CRON_H
#define VAR_SPOOL_CRON_H

#include <stdint.h>

typedef struct
{
    uint32_t cron_id;
    const char *cron_user;
    const char *cron_job;
    const char *cron_schedule;
    uint32_t is_active;
} var_spool_cron_entry_t;

typedef struct
{
    uint32_t total_cron_jobs;
    uint32_t active_cron_jobs;
    uint32_t disabled_jobs;
} var_spool_cron_state_t;

int var_spool_cron_init(void);
int var_spool_cron_list(var_spool_cron_entry_t *jobs, uint32_t *count);
int var_spool_cron_get_user_jobs(const char *user, var_spool_cron_entry_t *jobs, uint32_t *count);
int var_spool_cron_add_job(const char *user, const char *job, const char *schedule);
int var_spool_cron_remove_job(const char *user, const char *job);
int var_spool_cron_get_state(var_spool_cron_state_t *state);

#endif
