#ifndef KERNEL_FS_H
#define KERNEL_FS_H

#include <stdint.h>

typedef struct
{
    uint32_t fs_id;
    const char *fs_name;
    const char *mount_point;
    uint32_t block_size;
} kernel_fs_info_t;

typedef struct
{
    uint32_t fs_count;
    uint64_t total_size;
    uint64_t used_size;
} kernel_fs_state_t;

int kernel_fs_init(void);
int kernel_fs_register(kernel_fs_info_t *fs_info);
int kernel_fs_unregister(uint32_t fs_id);
int kernel_fs_mount(uint32_t fs_id, const char *mount_point);
int kernel_fs_unmount(uint32_t fs_id);
int kernel_fs_get_info(uint32_t fs_id, kernel_fs_info_t *info);

#endif
