#ifndef FRAMEWORKS_H
#define FRAMEWORKS_H

#include <stdint.h>

typedef struct
{
    const char *name;
    uint32_t version;
    void *handle;
} framework_t;

int frameworks_init(void);
int frameworks_load(const char *framework_name);
int frameworks_unload(const char *framework_name);
int frameworks_get_symbol(const char *framework_name, const char *symbol, void **ptr);
int frameworks_list(framework_t **frameworks, uint32_t *count);
int frameworks_validate(const char *framework_name);

#endif
