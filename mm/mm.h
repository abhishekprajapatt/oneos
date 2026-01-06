#ifndef MM_H
#define MM_H

#include <stdint.h>

typedef struct
{
    uint32_t total_memory;
    uint32_t free_memory;
    uint32_t used_memory;
} mm_stat_t;

int mm_init(uint32_t total_size);
int mm_malloc(uint32_t size, void **ptr);
int mm_free(void *ptr);
int mm_stat(mm_stat_t *stat);
int mm_page_alloc(uint32_t page_count, void **pages);
int mm_page_free(void *pages);

#endif
