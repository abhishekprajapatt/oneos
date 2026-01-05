#ifndef VAR_SPOOL_PRINT_H
#define VAR_SPOOL_PRINT_H

#include <stdint.h>

typedef struct
{
    uint32_t print_id;
    const char *job_name;
    const char *printer_name;
    const char *job_status;
    uint64_t job_size;
} var_spool_print_entry_t;

typedef struct
{
    uint32_t total_print_jobs;
    uint32_t queued_jobs;
    uint32_t printing_jobs;
    uint32_t completed_jobs;
} var_spool_print_state_t;

int var_spool_print_init(void);
int var_spool_print_list(var_spool_print_entry_t *jobs, uint32_t *count);
int var_spool_print_get_printer_queue(const char *printer_name, var_spool_print_entry_t *jobs, uint32_t *count);
int var_spool_print_submit_job(const char *job_name, const char *printer_name, const void *data, uint64_t size);
int var_spool_print_cancel_job(const char *job_name);
int var_spool_print_get_state(var_spool_print_state_t *state);

#endif
