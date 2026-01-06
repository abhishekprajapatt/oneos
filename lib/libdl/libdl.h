#ifndef LIBDL_H
#define LIBDL_H

#include <stdint.h>

int libdl_init(void);
int libdl_dlopen(const char *filename, int flags, void **handle);
int libdl_dlclose(void *handle);
int libdl_dlsym(void *handle, const char *symbol, void **symbol_ptr);
int libdl_dlerror(char *error_buffer, uint32_t buffer_size);
int libdl_dladdr(void *address, void **info);

#endif
