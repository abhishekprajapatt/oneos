#include "libz.h"

int libz_init(void)
{
    return 0;
}

int libz_compress(const void *src, uint32_t src_size, void *dst, uint32_t *dst_size)
{
    if (!src || src_size == 0 || !dst || !dst_size)
    {
        return -1;
    }
    return 0;
}

int libz_decompress(const void *src, uint32_t src_size, void *dst, uint32_t *dst_size)
{
    if (!src || src_size == 0 || !dst || !dst_size)
    {
        return -1;
    }
    return 0;
}

int libz_crc32(const void *buf, uint32_t size, uint32_t *crc)
{
    if (!buf || size == 0 || !crc)
    {
        return -1;
    }
    return 0;
}

int libz_version(char *version, uint32_t max_len)
{
    if (!version || max_len == 0)
    {
        return -1;
    }
    return 0;
}
