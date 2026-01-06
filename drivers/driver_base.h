#ifndef DRIVER_BASE_H
#define DRIVER_BASE_H

#include <stdint.h>

typedef struct
{
    const char *name;
    uint32_t version;
    int (*probe)(void);
    int (*remove)(void);
    int (*suspend)(void);
    int (*resume)(void);
} driver_t;

int driver_base_init(void);
int driver_register(driver_t *driver);
int driver_unregister(const char *name);
int driver_find(const char *name, driver_t **driver);
int driver_probe_all(void);

#endif
