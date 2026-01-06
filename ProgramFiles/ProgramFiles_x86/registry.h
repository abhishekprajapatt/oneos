#ifndef REGISTRY_H
#define REGISTRY_H

#include <stdint.h>

typedef struct
{
    const char *key;
    const char *value;
    uint32_t type;
} registry_entry_t;

int registry_init(void);
int registry_set_key(const char *path, const char *key, const char *value);
int registry_get_key(const char *path, const char *key, char *value, uint32_t max_len);
int registry_delete_key(const char *path, const char *key);
int registry_enumerate_keys(const char *path, registry_entry_t **entries, uint32_t *count);
int registry_backup(const char *output_file);

#endif
