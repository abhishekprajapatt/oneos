#ifndef INCLUDE_GNU_H
#define INCLUDE_GNU_H

#include <stdint.h>

typedef struct
{
    uint32_t hash_id;
    uint8_t hash_value[32];
} include_gnu_hash_t;

typedef struct
{
    const char *symbol_name;
    uint32_t symbol_value;
    uint32_t symbol_size;
} include_gnu_symbol_t;

int include_gnu_compute_hash(const char *string, include_gnu_hash_t *hash);
int include_gnu_lookup_symbol(const char *symbol_name, include_gnu_symbol_t *symbol);
int include_gnu_get_version(const char *library_name, char *version, uint32_t size);
int include_gnu_check_version(const char *library_name, const char *required_version);
int include_gnu_register_symbol(include_gnu_symbol_t *symbol);
int include_gnu_unregister_symbol(const char *symbol_name);

#endif
