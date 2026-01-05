#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

typedef struct
{
    uint32_t virt_addr;
    uint32_t phys_addr;
    uint32_t size;
    uint32_t flags;
} memory_region_t;

#define MEM_REMAP_NOCACHE 0x01
#define MEM_REMAP_WRITEONLY 0x02
#define MEM_REMAP_READONLY 0x04

#endif
