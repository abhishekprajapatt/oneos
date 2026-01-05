#ifndef VAR_RUN_H
#define VAR_RUN_H

#include <stdint.h>

typedef struct
{
    uint32_t pid_id;
    uint64_t process_id;
    const char *process_name;
    const char *pid_file_path;
    uint32_t is_running;
} var_run_entry_t;

typedef struct
{
    uint32_t total_pids;
    uint32_t active_processes;
    uint32_t inactive_processes;
} var_run_state_t;

int var_run_init(void);
int var_run_list(var_run_entry_t *entries, uint32_t *count);
int var_run_create_pidfile(uint64_t pid, const char *process_name);
int var_run_get_pidfile(const char *process_name, uint64_t *pid);
int var_run_remove_pidfile(const char *process_name);
int var_run_check_process(const char *process_name, uint32_t *running);
int var_run_get_state(var_run_state_t *state);

#endif
