#ifndef DRIVER_CORE_H
#define DRIVER_CORE_H

#include <stdint.h>
#include <stddef.h>

#define DRIVER_SUCCESS 0
#define DRIVER_ERROR -1
#define DRIVER_TIMEOUT -2
#define DRIVER_NOT_SUPPORTED -3

typedef struct
{
    const char *name;
    uint32_t version;
    uint32_t flags;
} driver_info_t;

typedef int (*driver_init_func)(void);
typedef int (*driver_probe_func)(void *dev);
typedef int (*driver_remove_func)(void *dev);
typedef void (*driver_shutdown_func)(void *dev);

typedef struct
{
    driver_info_t info;
    driver_init_func init;
    driver_probe_func probe;
    driver_remove_func remove;
    driver_shutdown_func shutdown;
} driver_t;

int register_driver(driver_t *driver);
int unregister_driver(const char *name);
driver_t *get_driver(const char *name);

#endif
