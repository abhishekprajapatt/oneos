#ifndef LIBGLIB_H
#define LIBGLIB_H

#include <stdint.h>

typedef struct
{
    void *data;
    void (*free_func)(void *);
} libglib_list_node_t;

int libglib_init(void);
int libglib_g_malloc(uint32_t size, void **ptr);
int libglib_g_free(void *ptr);
int libglib_g_list_append(libglib_list_node_t *list, void *data);
int libglib_g_string_new(const char *str, void **g_string);
int libglib_g_hash_table_new(void **hash_table);

#endif
