#ifndef LIBC_H
#define LIBC_H

#include <stdint.h>

int libc_init(void);
int libc_malloc_init(void);
int libc_stdio_init(void);
int libc_stdlib_init(void);
int libc_string_init(void);
int libc_set_locale(const char *locale);

#endif
