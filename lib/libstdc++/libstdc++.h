#ifndef LIBSTDCXX_H
#define LIBSTDCXX_H

#include <stdint.h>

int libstdcxx_init(void);
int libstdcxx_vector_create(void **vec);
int libstdcxx_vector_push_back(void *vec, void *elem);
int libstdcxx_string_create(const char *str, void **s);
int libstdcxx_map_create(void **map);
int libstdcxx_map_insert(void *map, const char *key, void *value);

#endif
