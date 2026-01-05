#ifndef DISK_UTILITY_H
#define DISK_UTILITY_H

#include <stdint.h>

typedef struct
{
    uint32_t disk_id;
    const char *disk_name;
    const char *mount_point;
    uint64_t total_size;
    uint64_t used_size;
    uint64_t free_size;
    const char *file_system;
} disk_info_t;

typedef struct
{
    uint32_t total_disks;
    uint32_t mounted_disks;
    uint64_t total_capacity;
    uint64_t used_capacity;
} disk_stats_t;

int disk_utility_init(void);
int disk_utility_list_disks(disk_info_t *disks, uint32_t *count);
int disk_utility_get_disk_info(uint32_t disk_id, disk_info_t *info);
int disk_utility_mount_disk(uint32_t disk_id);
int disk_utility_unmount_disk(uint32_t disk_id);
int disk_utility_verify_disk(uint32_t disk_id);
int disk_utility_repair_disk(uint32_t disk_id);
int disk_utility_get_stats(disk_stats_t *stats);

#endif
