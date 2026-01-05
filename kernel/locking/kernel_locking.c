#include "kernel_locking.h"

int kernel_locking_init(void)
{
    return 0;
}

int kernel_locking_create_lock(uint32_t lock_type, kernel_locking_lock_t *lock)
{
    if (!lock)
    {
        return -1;
    }
    return 0;
}

int kernel_locking_destroy_lock(uint32_t lock_id)
{
    return 0;
}

int kernel_locking_acquire(uint32_t lock_id)
{
    return 0;
}

int kernel_locking_release(uint32_t lock_id)
{
    return 0;
}

int kernel_locking_trylock(uint32_t lock_id)
{
    return 0;
}
