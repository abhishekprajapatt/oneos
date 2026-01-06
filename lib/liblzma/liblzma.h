#ifndef LIBLZMA_H
#define LIBLZMA_H

#include <stdint.h>

int liblzma_init(void);
int liblzma_compress(const void *src, uint32_t src_size, void *dst, uint32_t *dst_size);
int liblzma_decompress(const void *src, uint32_t src_size, void *dst, uint32_t *dst_size);
int liblzma_set_preset(uint32_t preset);
int liblzma_get_version(char *version, uint32_t max_len);

#endif
