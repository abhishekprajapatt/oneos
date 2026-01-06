#ifndef LIBSELINUX_H
#define LIBSELINUX_H

#include <stdint.h>

int libselinux_init(void);
int libselinux_is_selinux_enabled(void);
int libselinux_getcon(char *con, uint32_t con_len);
int libselinux_setcon(const char *con);
int libselinux_getfilecon(const char *path, char *con, uint32_t con_len);
int libselinux_setfilecon(const char *path, const char *con);

#endif
