#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include <stdint.h>

typedef struct
{
    const char *name;
    const char *path;
    uint32_t version;
} extension_t;

int extensions_init(void);
int extensions_load(const char *extension_path);
int extensions_unload(const char *extension_name);
int extensions_list(extension_t **extensions, uint32_t *count);
int extensions_enable(const char *extension_name);
int extensions_disable(const char *extension_name);

#endif
