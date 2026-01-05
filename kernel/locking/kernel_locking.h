#ifndef KERNEL_LOCKING_H
#define KERNEL_LOCKING_H

#include <stdint.h>

typedef struct
{
    uint32_t lock_id;
    uint32_t owner_pid;
    uint32_t lock_type;
    uint32_t acquired_count;
} kernel_locking_lock_t;

typedef struct
{
    uint32_t lock_count;
    uint32_t active_locks;
    uint32_t waiting_threads;
} kernel_locking_state_t;

int kernel_locking_init(void);
int kernel_locking_create_lock(uint32_t lock_type, kernel_locking_lock_t *lock);
int kernel_locking_destroy_lock(uint32_t lock_id);
int kernel_locking_acquire(uint32_t lock_id);
int kernel_locking_release(uint32_t lock_id);
int kernel_locking_trylock(uint32_t lock_id);

#endif
