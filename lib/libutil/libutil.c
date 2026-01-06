#include "libutil.h"

int libutil_init(void)
{
    return 0;
}

int libutil_login_tty(int fd)
{
    if (fd < 0)
    {
        return -1;
    }
    return 0;
}

int libutil_openpty(int *master, int *slave, char *name, void *termp, void *winp)
{
    if (!master || !slave)
    {
        return -1;
    }
    return 0;
}

int libutil_forkpty(int *master, char *name, void *termp, void *winp)
{
    if (!master)
    {
        return -1;
    }
    return 0;
}
