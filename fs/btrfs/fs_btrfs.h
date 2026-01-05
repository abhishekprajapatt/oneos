#ifndef FS_BTRFS_H
#define FS_BTRFS_H

#include <stdint.h>

typedef struct
{
    uint32_t fs_id;
    uint64_t block_size;
    uint64_t total_size;
    uint32_t flags;
} fs_btrfs_device_t;

typedef struct
{
    uint64_t inode_num;
    uint32_t mode;
    uint64_t size;
    uint64_t ctime;
} fs_btrfs_inode_t;

int fs_btrfs_init(void);
int fs_btrfs_mount(fs_btrfs_device_t *dev);
int fs_btrfs_unmount(fs_btrfs_device_t *dev);
int fs_btrfs_read_inode(fs_btrfs_device_t *dev, fs_btrfs_inode_t *inode);
int fs_btrfs_write_inode(fs_btrfs_device_t *dev, fs_btrfs_inode_t *inode);
int fs_btrfs_read_block(fs_btrfs_device_t *dev, uint64_t block, uint8_t *buffer);

#endif
