#include "liblzma.h"

int liblzma_init(void)
{
    return 0;
}

int liblzma_compress(const void *src, uint32_t src_size, void *dst, uint32_t *dst_size)
{
    if (!src || src_size == 0 || !dst || !dst_size)
    {
        return -1;
    }
    return 0;
}

int liblzma_decompress(const void *src, uint32_t src_size, void *dst, uint32_t *dst_size)
{
    if (!src || src_size == 0 || !dst || !dst_size)
    {
        return -1;
    }
    return 0;
}

int liblzma_set_preset(uint32_t preset)
{
    if (preset == 0)
    {
        return -1;
    }
    return 0;
}

int liblzma_get_version(char *version, uint32_t max_len)
{
    if (!version || max_len == 0)
    {
        return -1;
    }
    return 0;
}
