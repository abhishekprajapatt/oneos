#include "disk_utility.h"

int disk_utility_init(void)
{
    return 0;
}

int disk_utility_list_disks(disk_info_t *disks, uint32_t *count)
{
    if (!disks || !count)
    {
        return -1;
    }
    return 0;
}

int disk_utility_get_disk_info(uint32_t disk_id, disk_info_t *info)
{
    if (disk_id == 0 || !info)
    {
        return -1;
    }
    return 0;
}

int disk_utility_mount_disk(uint32_t disk_id)
{
    if (disk_id == 0)
    {
        return -1;
    }
    return 0;
}

int disk_utility_unmount_disk(uint32_t disk_id)
{
    if (disk_id == 0)
    {
        return -1;
    }
    return 0;
}

int disk_utility_verify_disk(uint32_t disk_id)
{
    if (disk_id == 0)
    {
        return -1;
    }
    return 0;
}

int disk_utility_repair_disk(uint32_t disk_id)
{
    if (disk_id == 0)
    {
        return -1;
    }
    return 0;
}

int disk_utility_get_stats(disk_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
