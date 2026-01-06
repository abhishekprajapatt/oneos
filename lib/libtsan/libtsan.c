#include "libtsan.h"

int libtsan_init(void)
{
    return 0;
}

int libtsan_read(void *addr, uint32_t size)
{
    if (!addr || size == 0)
    {
        return -1;
    }
    return 0;
}

int libtsan_write(void *addr, uint32_t size)
{
    if (!addr || size == 0)
    {
        return -1;
    }
    return 0;
}

int libtsan_func_entry(void *pc)
{
    if (!pc)
    {
        return -1;
    }
    return 0;
}

int libtsan_func_exit(void)
{
    return 0;
}

int libtsan_ignore_reads_begin(void)
{
    return 0;
}

int libtsan_ignore_reads_end(void)
{
    return 0;
}
