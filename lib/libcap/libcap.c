#include "libcap.h"

int libcap_init(void)
{
    return 0;
}

int libcap_get_current(libcap_t *cap)
{
    if (!cap)
    {
        return -1;
    }
    return 0;
}

int libcap_set_current(libcap_t *cap)
{
    if (!cap)
    {
        return -1;
    }
    return 0;
}

int libcap_drop_capability(uint32_t capability)
{
    if (capability == 0)
    {
        return -1;
    }
    return 0;
}

int libcap_clear(void)
{
    return 0;
}
