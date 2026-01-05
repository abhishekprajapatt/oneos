#ifndef FS_ZONEFS_H
#define FS_ZONEFS_H

#include <stdint.h>

typedef struct
{
    uint32_t fs_id;
    uint32_t zone_size;
    uint32_t zone_count;
    uint32_t flags;
} fs_zonefs_device_t;

typedef struct
{
    uint32_t zone_num;
    uint32_t zone_type;
    uint64_t zone_size;
    uint32_t wp_offset;
} fs_zonefs_zone_t;

int fs_zonefs_init(void);
int fs_zonefs_mount(fs_zonefs_device_t *dev);
int fs_zonefs_unmount(fs_zonefs_device_t *dev);
int fs_zonefs_get_zone_info(fs_zonefs_device_t *dev, uint32_t zone_num, fs_zonefs_zone_t *zone);
int fs_zonefs_reset_zone(fs_zonefs_device_t *dev, uint32_t zone_num);
int fs_zonefs_write_zone(fs_zonefs_device_t *dev, uint32_t zone_num, uint8_t *data, uint32_t size);

#endif
