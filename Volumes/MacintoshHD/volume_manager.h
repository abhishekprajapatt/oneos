#ifndef VOLUME_MANAGER_H
#define VOLUME_MANAGER_H

#include <stdint.h>

typedef struct
{
    const char *volume_name;
    uint64_t total_size;
    uint64_t used_size;
    uint64_t free_size;
    uint32_t block_size;
} volume_info_t;

int volume_manager_init(void);
int volume_manager_mount_volume(const char *volume_path, const char *mount_point);
int volume_manager_unmount_volume(const char *mount_point);
int volume_manager_get_info(const char *volume_name, volume_info_t *info);
int volume_manager_list_volumes(char **volumes, uint32_t max_volumes);
int volume_manager_format_volume(const char *volume_path, const char *fs_type);
int volume_manager_check_volume(const char *volume_path);

#endif
