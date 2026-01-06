#include "librt-static.h"

int librt_static_init(void)
{
    return 0;
}

int librt_static_timer_create(clockid_t clockid, void *sevp, timer_t **timerid)
{
    if (!timerid)
    {
        return -1;
    }
    return 0;
}

int librt_static_clock_gettime(uint32_t clockid, void *tp)
{
    if (!tp)
    {
        return -1;
    }
    return 0;
}
