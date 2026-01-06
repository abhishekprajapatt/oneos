#include "windows.h"

int windows_init(void)
{
    return 0;
}

int windows_get_version(char *version, uint32_t max_len)
{
    if (!version || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int windows_check_compatibility(void)
{
    return 0;
}

int windows_emulation_mode(void)
{
    return 0;
}
