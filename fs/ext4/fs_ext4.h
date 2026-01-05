#ifndef FS_EXT4_H
#define FS_EXT4_H

#include <stdint.h>

typedef struct
{
    uint32_t fs_id;
    uint32_t block_size;
    uint64_t extent_tree;
    uint32_t flags;
} fs_ext4_device_t;

typedef struct
{
    uint32_t inode_num;
    uint16_t mode;
    uint64_t size;
    uint64_t ctime;
    uint64_t mtime;
    uint64_t atime;
} fs_ext4_inode_t;

int fs_ext4_init(void);
int fs_ext4_mount(fs_ext4_device_t *dev);
int fs_ext4_unmount(fs_ext4_device_t *dev);
int fs_ext4_read_inode(fs_ext4_device_t *dev, fs_ext4_inode_t *inode);
int fs_ext4_write_inode(fs_ext4_device_t *dev, fs_ext4_inode_t *inode);
int fs_ext4_find_extent(fs_ext4_device_t *dev, uint32_t inode, uint64_t block);

#endif
