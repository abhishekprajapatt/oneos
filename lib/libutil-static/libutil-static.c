#include "libutil-static.h"

int libutil_static_init(void)
{
    return 0;
}

int libutil_static_login_tty(int fd)
{
    if (fd < 0)
    {
        return -1;
    }
    return 0;
}

int libutil_static_openpty(int *master, int *slave, char *name)
{
    if (!master || !slave)
    {
        return -1;
    }
    return 0;
}
