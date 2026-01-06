#ifndef LIBGC_H
#define LIBGC_H

#include <stdint.h>

int libgc_init(void);
int libgc_malloc(uint32_t size, void **ptr);
int libgc_free(void *ptr);
int libgc_collect(void);
int libgc_get_heap_size(uint64_t *size);
int libgc_disable_collection(void);
int libgc_enable_collection(void);

#endif
