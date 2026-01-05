#ifndef VAR_RUN_PID_H
#define VAR_RUN_PID_H

#include <stdint.h>

typedef struct
{
    uint32_t pid_entry_id;
    const char *process_name;
    uint64_t process_id;
    const char *pid_file_path;
    uint64_t creation_time;
} var_run_pid_entry_t;

typedef struct
{
    uint32_t total_pid_files;
    uint32_t active_pid_files;
    uint32_t orphaned_pid_files;
} var_run_pid_state_t;

int var_run_pid_init(void);
int var_run_pid_list(var_run_pid_entry_t *pids, uint32_t *count);
int var_run_pid_create(const char *process_name, uint64_t process_id);
int var_run_pid_get(const char *process_name, var_run_pid_entry_t *pid);
int var_run_pid_remove(const char *process_name);
int var_run_pid_verify(const char *process_name, uint32_t *exists);
int var_run_pid_get_state(var_run_pid_state_t *state);

#endif
