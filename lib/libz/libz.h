#ifndef LIBZ_H
#define LIBZ_H

#include <stdint.h>

int libz_init(void);
int libz_compress(const void *src, uint32_t src_size, void *dst, uint32_t *dst_size);
int libz_decompress(const void *src, uint32_t src_size, void *dst, uint32_t *dst_size);
int libz_crc32(const void *buf, uint32_t size, uint32_t *crc);
int libz_version(char *version, uint32_t max_len);

#endif
