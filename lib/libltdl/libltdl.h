#ifndef LIBLTDL_H
#define LIBLTDL_H

#include <stdint.h>

typedef void libltdl_handle;

int libltdl_init(void);
int libltdl_dlopen(const char *filename, libltdl_handle **handle);
int libltdl_dlclose(libltdl_handle *handle);
int libltdl_dlsym(libltdl_handle *handle, const char *symbol, void **symbol_ptr);
int libltdl_dlerror(char *error_buffer, uint32_t buffer_size);

#endif
