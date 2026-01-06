#ifndef LIBTSAN_H
#define LIBTSAN_H

#include <stdint.h>

int libtsan_init(void);
int libtsan_read(void *addr, uint32_t size);
int libtsan_write(void *addr, uint32_t size);
int libtsan_func_entry(void *pc);
int libtsan_func_exit(void);
int libtsan_ignore_reads_begin(void);
int libtsan_ignore_reads_end(void);

#endif
