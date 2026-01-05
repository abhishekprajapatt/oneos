#ifndef VAR_RUN_LOCK_H
#define VAR_RUN_LOCK_H

#include <stdint.h>

typedef struct
{
    uint32_t lock_id;
    const char *lock_name;
    uint64_t lock_owner_pid;
    const char *lock_path;
    uint32_t is_locked;
} var_run_lock_entry_t;

typedef struct
{
    uint32_t total_locks;
    uint32_t active_locks;
    uint32_t stale_locks;
} var_run_lock_state_t;

int var_run_lock_init(void);
int var_run_lock_list(var_run_lock_entry_t *locks, uint32_t *count);
int var_run_lock_acquire(const char *lock_name, uint64_t owner_pid);
int var_run_lock_release(const char *lock_name);
int var_run_lock_is_locked(const char *lock_name, uint32_t *locked);
int var_run_lock_cleanup_stale(void);
int var_run_lock_get_state(var_run_lock_state_t *state);

#endif
