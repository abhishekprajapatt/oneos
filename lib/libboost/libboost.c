#include "libboost.h"

int libboost_init(void)
{
    return 0;
}

int libboost_system_init(void)
{
    return 0;
}

int libboost_thread_init(void)
{
    return 0;
}

int libboost_filesystem_init(void)
{
    return 0;
}

int libboost_get_version(char *version, uint32_t max_len)
{
    if (!version || max_len == 0)
    {
        return -1;
    }
    return 0;
}
