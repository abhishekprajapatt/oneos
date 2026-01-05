#ifndef FS_XFS_H
#define FS_XFS_H

#include <stdint.h>

typedef struct
{
    uint32_t fs_id;
    uint32_t block_size;
    uint32_t blocklog;
    uint64_t total_blocks;
    uint32_t flags;
} fs_xfs_device_t;

typedef struct
{
    uint32_t inode_num;
    uint16_t mode;
    uint64_t size;
    uint64_t mtime;
} fs_xfs_inode_t;

int fs_xfs_init(void);
int fs_xfs_mount(fs_xfs_device_t *dev);
int fs_xfs_unmount(fs_xfs_device_t *dev);
int fs_xfs_read_inode(fs_xfs_device_t *dev, fs_xfs_inode_t *inode);
int fs_xfs_write_inode(fs_xfs_device_t *dev, fs_xfs_inode_t *inode);
int fs_xfs_allocate_extent(fs_xfs_device_t *dev, uint64_t *extent);

#endif
