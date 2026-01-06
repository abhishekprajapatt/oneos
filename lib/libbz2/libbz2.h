#ifndef LIBBZ2_H
#define LIBBZ2_H

#include <stdint.h>

int libbz2_compress(const void *src, uint32_t src_size, void *dst, uint32_t *dst_size);
int libbz2_decompress(const void *src, uint32_t src_size, void *dst, uint32_t *dst_size);
int libbz2_init(void);
int libbz2_set_block_size(uint32_t block_size);
int libbz2_get_version(char *version, uint32_t max_len);

#endif
