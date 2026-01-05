#ifndef FS_EXT3_H
#define FS_EXT3_H

#include <stdint.h>

typedef struct
{
    uint32_t fs_id;
    uint32_t block_size;
    uint64_t journal_blocks;
    uint32_t flags;
} fs_ext3_device_t;

typedef struct
{
    uint32_t inode_num;
    uint16_t mode;
    uint32_t size;
    uint64_t ctime;
    uint64_t mtime;
} fs_ext3_inode_t;

int fs_ext3_init(void);
int fs_ext3_mount(fs_ext3_device_t *dev);
int fs_ext3_unmount(fs_ext3_device_t *dev);
int fs_ext3_read_inode(fs_ext3_device_t *dev, fs_ext3_inode_t *inode);
int fs_ext3_write_inode(fs_ext3_device_t *dev, fs_ext3_inode_t *inode);
int fs_ext3_recover_journal(fs_ext3_device_t *dev);

#endif
