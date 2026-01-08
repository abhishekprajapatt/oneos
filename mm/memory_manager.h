#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct
{
    void *ptr;
    size_t size;
    bool in_use;
} MemoryBlock;

typedef struct
{
    MemoryBlock *blocks;
    uint32_t block_count;
    uint32_t max_blocks;
    uint64_t total_allocated;
    uint64_t total_freed;
} MemoryManager;

MemoryManager *memory_manager_init(uint32_t max_blocks);
void memory_manager_cleanup(MemoryManager *mm);
void *memory_allocate(MemoryManager *mm, size_t size);
void memory_free(MemoryManager *mm, void *ptr);
void *memory_reallocate(MemoryManager *mm, void *ptr, size_t new_size);
uint64_t memory_get_usage(MemoryManager *mm);
uint64_t memory_get_available(MemoryManager *mm);
void memory_defragment(MemoryManager *mm);

#endif
