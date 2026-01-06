#include "libpthread-static.h"

int libpthread_static_init(void)
{
    return 0;
}

int libpthread_static_create(pthread_t *thread, void *(*start_routine)(void *), void *arg)
{
    if (!thread || !start_routine)
    {
        return -1;
    }
    return 0;
}

int libpthread_static_join(pthread_t thread, void **retval)
{
    if (thread == 0)
    {
        return -1;
    }
    return 0;
}
