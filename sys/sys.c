#include "sys.h"

int sys_init(void)
{
    return 0;
}

int sys_get_info(sys_info_t *info)
{
    if (!info)
    {
        return -1;
    }
    info->uptime = 0;
    info->load_avg[0] = 0;
    info->load_avg[1] = 0;
    info->load_avg[2] = 0;
    info->total_ram = 0x80000000;
    info->free_ram = 0x40000000;
    return 0;
}

int sys_set_hostname(const char *hostname)
{
    if (!hostname)
    {
        return -1;
    }
    return 0;
}

int sys_get_hostname(char *hostname, uint32_t max_len)
{
    if (!hostname || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int sys_reboot(void)
{
    return 0;
}

int sys_shutdown(void)
{
    return 0;
}
