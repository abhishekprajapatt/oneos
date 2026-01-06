#ifndef LIB_CORE_H
#define LIB_CORE_H

#include <stdint.h>

typedef struct
{
    uint32_t allocated;
    uint32_t freed;
    uint32_t total;
} lib_memory_stat_t;

int lib_core_init(void);
int lib_core_malloc(uint32_t size, void **ptr);
int lib_core_free(void *ptr);
int lib_core_realloc(void *ptr, uint32_t new_size, void **new_ptr);
int lib_core_memcpy(void *dst, const void *src, uint32_t size);
int lib_core_memset(void *dst, int value, uint32_t size);
int lib_core_get_memory_stats(lib_memory_stat_t *stats);
int lib_core_shutdown(void);

#endif
