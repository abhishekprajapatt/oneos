#ifndef LIBPTHREAD_H
#define LIBPTHREAD_H

#include <stdint.h>

typedef uint64_t pthread_t;

int libpthread_init(void);
int libpthread_create(pthread_t *thread, void *(*start_routine)(void *), void *arg);
int libpthread_join(pthread_t thread, void **retval);
int libpthread_cancel(pthread_t thread);
int libpthread_mutex_init(void **mutex);
int libpthread_mutex_lock(void *mutex);
int libpthread_mutex_unlock(void *mutex);

#endif
