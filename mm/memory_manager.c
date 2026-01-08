#include "memory_manager.h"
#include <stdlib.h>
#include <string.h>

MemoryManager *memory_manager_init(uint32_t max_blocks)
{
    MemoryManager *mm = (MemoryManager *)malloc(sizeof(MemoryManager));
    if (!mm)
        return NULL;

    mm->max_blocks = max_blocks;
    mm->block_count = 0;
    mm->total_allocated = 0;
    mm->total_freed = 0;

    mm->blocks = (MemoryBlock *)malloc(max_blocks * sizeof(MemoryBlock));
    if (!mm->blocks)
    {
        free(mm);
        return NULL;
    }

    memset(mm->blocks, 0, max_blocks * sizeof(MemoryBlock));

    return mm;
}

void memory_manager_cleanup(MemoryManager *mm)
{
    if (!mm)
        return;

    for (uint32_t i = 0; i < mm->block_count; i++)
    {
        if (mm->blocks[i].in_use && mm->blocks[i].ptr)
        {
            free(mm->blocks[i].ptr);
        }
    }

    free(mm->blocks);
    free(mm);
}

void *memory_allocate(MemoryManager *mm, size_t size)
{
    if (!mm || size == 0 || mm->block_count >= mm->max_blocks)
        return NULL;

    void *ptr = malloc(size);
    if (!ptr)
        return NULL;

    mm->blocks[mm->block_count].ptr = ptr;
    mm->blocks[mm->block_count].size = size;
    mm->blocks[mm->block_count].in_use = true;
    mm->block_count++;

    mm->total_allocated += size;

    return ptr;
}

void memory_free(MemoryManager *mm, void *ptr)
{
    if (!mm || !ptr)
        return;

    for (uint32_t i = 0; i < mm->block_count; i++)
    {
        if (mm->blocks[i].ptr == ptr && mm->blocks[i].in_use)
        {
            mm->total_freed += mm->blocks[i].size;
            mm->blocks[i].in_use = false;
            free(ptr);
            break;
        }
    }
}

void *memory_reallocate(MemoryManager *mm, void *ptr, size_t new_size)
{
    if (!mm || new_size == 0)
        return NULL;

    for (uint32_t i = 0; i < mm->block_count; i++)
    {
        if (mm->blocks[i].ptr == ptr && mm->blocks[i].in_use)
        {
            void *new_ptr = realloc(ptr, new_size);
            if (!new_ptr)
                return NULL;

            mm->total_allocated += (new_size - mm->blocks[i].size);
            mm->blocks[i].ptr = new_ptr;
            mm->blocks[i].size = new_size;

            return new_ptr;
        }
    }

    return NULL;
}

uint64_t memory_get_usage(MemoryManager *mm)
{
    if (!mm)
        return 0;
    return mm->total_allocated - mm->total_freed;
}

uint64_t memory_get_available(MemoryManager *mm)
{
    if (!mm)
        return 0;
    return 8 * 1024 * 1024 * 1024 - memory_get_usage(mm);
}

void memory_defragment(MemoryManager *mm)
{
    if (!mm)
        return;
}
