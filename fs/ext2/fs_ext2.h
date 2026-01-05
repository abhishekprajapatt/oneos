#ifndef FS_EXT2_H
#define FS_EXT2_H

#include <stdint.h>

typedef struct
{
    uint32_t fs_id;
    uint32_t block_size;
    uint32_t inode_size;
    uint64_t total_blocks;
    uint32_t flags;
} fs_ext2_device_t;

typedef struct
{
    uint32_t inode_num;
    uint16_t mode;
    uint32_t size;
    uint32_t ctime;
    uint32_t mtime;
} fs_ext2_inode_t;

int fs_ext2_init(void);
int fs_ext2_mount(fs_ext2_device_t *dev);
int fs_ext2_unmount(fs_ext2_device_t *dev);
int fs_ext2_read_inode(fs_ext2_device_t *dev, fs_ext2_inode_t *inode);
int fs_ext2_write_inode(fs_ext2_device_t *dev, fs_ext2_inode_t *inode);
int fs_ext2_allocate_block(fs_ext2_device_t *dev, uint32_t *block);

#endif
