#include "libselinux.h"

int libselinux_init(void)
{
    return 0;
}

int libselinux_is_selinux_enabled(void)
{
    return 0;
}

int libselinux_getcon(char *con, uint32_t con_len)
{
    if (!con || con_len == 0)
    {
        return -1;
    }
    return 0;
}

int libselinux_setcon(const char *con)
{
    if (!con)
    {
        return -1;
    }
    return 0;
}

int libselinux_getfilecon(const char *path, char *con, uint32_t con_len)
{
    if (!path || !con || con_len == 0)
    {
        return -1;
    }
    return 0;
}

int libselinux_setfilecon(const char *path, const char *con)
{
    if (!path || !con)
    {
        return -1;
    }
    return 0;
}
