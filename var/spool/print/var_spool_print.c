#include "var_spool_print.h"

int var_spool_print_init(void)
{
    return 0;
}

int var_spool_print_list(var_spool_print_entry_t *jobs, uint32_t *count)
{
    if (!jobs || !count)
    {
        return -1;
    }
    return 0;
}

int var_spool_print_get_printer_queue(const char *printer_name, var_spool_print_entry_t *jobs, uint32_t *count)
{
    if (!printer_name || !jobs || !count)
    {
        return -1;
    }
    return 0;
}

int var_spool_print_submit_job(const char *job_name, const char *printer_name, const void *data, uint64_t size)
{
    if (!job_name || !printer_name || !data)
    {
        return -1;
    }
    return 0;
}

int var_spool_print_cancel_job(const char *job_name)
{
    if (!job_name)
    {
        return -1;
    }
    return 0;
}

int var_spool_print_get_state(var_spool_print_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
