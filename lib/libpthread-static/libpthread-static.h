#ifndef LIBPTHREAD_STATIC_H
#define LIBPTHREAD_STATIC_H

#include <stdint.h>

typedef uint64_t pthread_t;

int libpthread_static_init(void);
int libpthread_static_create(pthread_t *thread, void *(*start_routine)(void *), void *arg);
int libpthread_static_join(pthread_t thread, void **retval);

#endif
