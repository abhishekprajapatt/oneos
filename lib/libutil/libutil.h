#ifndef LIBUTIL_H
#define LIBUTIL_H

int libutil_init(void);
int libutil_login_tty(int fd);
int libutil_openpty(int *master, int *slave, char *name, void *termp, void *winp);
int libutil_forkpty(int *master, char *name, void *termp, void *winp);

#endif
