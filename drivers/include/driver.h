#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>

typedef int (*driver_probe_t)(void *dev);
typedef int (*driver_remove_t)(void *dev);
typedef int (*driver_suspend_t)(void *dev);
typedef int (*driver_resume_t)(void *dev);

typedef struct
{
    const char *name;
    const char *version;
    driver_probe_t probe;
    driver_remove_t remove;
    driver_suspend_t suspend;
    driver_resume_t resume;
} driver_ops_t;

typedef struct
{
    uint32_t id;
    const char *name;
    driver_ops_t ops;
    uint32_t flags;
} driver_t;

#endif
