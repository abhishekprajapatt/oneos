#ifndef VAR_SPOOL_H
#define VAR_SPOOL_H

#include <stdint.h>

typedef struct
{
    uint32_t spool_id;
    const char *job_name;
    const char *job_type;
    const char *job_path;
    uint32_t job_status;
} var_spool_entry_t;

typedef struct
{
    uint32_t total_jobs;
    uint32_t queued_jobs;
    uint32_t processing_jobs;
    uint32_t completed_jobs;
} var_spool_state_t;

int var_spool_init(void);
int var_spool_list(var_spool_entry_t *jobs, uint32_t *count);
int var_spool_get_job(const char *job_name, var_spool_entry_t *job);
int var_spool_queue_job(const char *name, const char *type, const char *data);
int var_spool_process_job(const char *job_name);
int var_spool_remove_job(const char *job_name);
int var_spool_get_state(var_spool_state_t *state);

#endif
