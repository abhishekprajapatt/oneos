#ifndef LIBSELINUX_STATIC_H
#define LIBSELINUX_STATIC_H

#include <stdint.h>

int libselinux_static_init(void);
int libselinux_static_is_enabled(void);
int libselinux_static_getcon(char *con, uint32_t con_len);
int libselinux_static_setcon(const char *con);

#endif
