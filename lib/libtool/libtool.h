#ifndef LIBTOOL_H
#define LIBTOOL_H

#include <stdint.h>

typedef void libtool_handle;

int libtool_init(void);
int libtool_open(const char *filename, libtool_handle **handle);
int libtool_close(libtool_handle *handle);
int libtool_dlsym(libtool_handle *handle, const char *symbol, void **symbol_ptr);
int libtool_get_error(char *error_buf, uint32_t buf_size);

#endif
