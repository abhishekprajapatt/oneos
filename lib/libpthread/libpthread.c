#include "libpthread.h"

int libpthread_init(void)
{
    return 0;
}

int libpthread_create(pthread_t *thread, void *(*start_routine)(void *), void *arg)
{
    if (!thread || !start_routine)
    {
        return -1;
    }
    return 0;
}

int libpthread_join(pthread_t thread, void **retval)
{
    if (thread == 0)
    {
        return -1;
    }
    return 0;
}

int libpthread_cancel(pthread_t thread)
{
    if (thread == 0)
    {
        return -1;
    }
    return 0;
}

int libpthread_mutex_init(void **mutex)
{
    if (!mutex)
    {
        return -1;
    }
    return 0;
}

int libpthread_mutex_lock(void *mutex)
{
    if (!mutex)
    {
        return -1;
    }
    return 0;
}

int libpthread_mutex_unlock(void *mutex)
{
    if (!mutex)
    {
        return -1;
    }
    return 0;
}
