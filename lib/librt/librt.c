#include "librt.h"

int librt_init(void)
{
    return 0;
}

int librt_timer_create(clockid_t clockid, void *sevp, timer_t **timerid)
{
    if (!timerid)
    {
        return -1;
    }
    return 0;
}

int librt_timer_delete(timer_t *timerid)
{
    if (!timerid)
    {
        return -1;
    }
    return 0;
}

int librt_timer_settime(timer_t *timerid, int flags, const void *new_value, void *old_value)
{
    if (!timerid || !new_value)
    {
        return -1;
    }
    return 0;
}

int librt_clock_gettime(uint32_t clockid, void *tp)
{
    if (!tp)
    {
        return -1;
    }
    return 0;
}
