#ifndef FS_VFAT_H
#define FS_VFAT_H

#include <stdint.h>

typedef struct
{
    uint32_t fs_id;
    uint32_t sector_size;
    uint32_t cluster_size;
    uint32_t fat_count;
    uint32_t flags;
} fs_vfat_device_t;

typedef struct
{
    uint32_t cluster;
    uint32_t size;
    uint16_t date;
    uint16_t time;
    uint8_t attributes;
} fs_vfat_inode_t;

int fs_vfat_init(void);
int fs_vfat_mount(fs_vfat_device_t *dev);
int fs_vfat_read_fat(fs_vfat_device_t *dev, uint32_t fat_entry);
int fs_vfat_read_cluster(fs_vfat_device_t *dev, uint32_t cluster, uint8_t *buffer);
int fs_vfat_read_inode(fs_vfat_device_t *dev, fs_vfat_inode_t *inode);
int fs_vfat_read_file(fs_vfat_device_t *dev, fs_vfat_inode_t *inode, uint8_t *buffer, uint32_t size);

#endif
