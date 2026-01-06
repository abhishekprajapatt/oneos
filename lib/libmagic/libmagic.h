#ifndef LIBMAGIC_H
#define LIBMAGIC_H

#include <stdint.h>

typedef void magic_t;

int libmagic_init(void);
int libmagic_open(int flags, magic_t **magic);
int libmagic_close(magic_t *magic);
int libmagic_load(magic_t *magic, const char *magicfile);
int libmagic_buffer(magic_t *magic, const void *buf, uint32_t size, char *result, uint32_t max_len);
int libmagic_file(magic_t *magic, const char *filename, char *result, uint32_t max_len);

#endif
