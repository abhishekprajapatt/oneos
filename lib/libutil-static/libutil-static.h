#ifndef LIBUTIL_STATIC_H
#define LIBUTIL_STATIC_H

int libutil_static_init(void);
int libutil_static_login_tty(int fd);
int libutil_static_openpty(int *master, int *slave, char *name);

#endif
