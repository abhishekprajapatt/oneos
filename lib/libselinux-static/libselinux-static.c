#include "libselinux-static.h"

int libselinux_static_init(void)
{
    return 0;
}

int libselinux_static_is_enabled(void)
{
    return 0;
}

int libselinux_static_getcon(char *con, uint32_t con_len)
{
    if (!con || con_len == 0)
    {
        return -1;
    }
    return 0;
}

int libselinux_static_setcon(const char *con)
{
    if (!con)
    {
        return -1;
    }
    return 0;
}
