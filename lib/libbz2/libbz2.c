#include "libbz2.h"

int libbz2_compress(const void *src, uint32_t src_size, void *dst, uint32_t *dst_size)
{
    if (!src || src_size == 0 || !dst || !dst_size)
    {
        return -1;
    }
    return 0;
}

int libbz2_decompress(const void *src, uint32_t src_size, void *dst, uint32_t *dst_size)
{
    if (!src || src_size == 0 || !dst || !dst_size)
    {
        return -1;
    }
    return 0;
}

int libbz2_init(void)
{
    return 0;
}

int libbz2_set_block_size(uint32_t block_size)
{
    if (block_size == 0)
    {
        return -1;
    }
    return 0;
}

int libbz2_get_version(char *version, uint32_t max_len)
{
    if (!version || max_len == 0)
    {
        return -1;
    }
    return 0;
}
