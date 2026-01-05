#ifndef FS_F2FS_H
#define FS_F2FS_H

#include <stdint.h>

typedef struct
{
    uint32_t fs_id;
    uint32_t block_size;
    uint32_t segs_per_sec;
    uint64_t total_segments;
    uint32_t flags;
} fs_f2fs_device_t;

typedef struct
{
    uint32_t inode_num;
    uint16_t mode;
    uint64_t size;
    uint64_t mtime;
} fs_f2fs_inode_t;

int fs_f2fs_init(void);
int fs_f2fs_mount(fs_f2fs_device_t *dev);
int fs_f2fs_unmount(fs_f2fs_device_t *dev);
int fs_f2fs_read_inode(fs_f2fs_device_t *dev, fs_f2fs_inode_t *inode);
int fs_f2fs_write_inode(fs_f2fs_device_t *dev, fs_f2fs_inode_t *inode);
int fs_f2fs_allocate_segment(fs_f2fs_device_t *dev, uint32_t *segment);

#endif
