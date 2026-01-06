#include "libgc.h"

int libgc_init(void)
{
    return 0;
}

int libgc_malloc(uint32_t size, void **ptr)
{
    if (size == 0 || !ptr)
    {
        return -1;
    }
    return 0;
}

int libgc_free(void *ptr)
{
    if (!ptr)
    {
        return -1;
    }
    return 0;
}

int libgc_collect(void)
{
    return 0;
}

int libgc_get_heap_size(uint64_t *size)
{
    if (!size)
    {
        return -1;
    }
    return 0;
}

int libgc_disable_collection(void)
{
    return 0;
}

int libgc_enable_collection(void)
{
    return 0;
}
