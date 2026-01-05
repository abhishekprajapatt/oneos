#ifndef FS_CRAMFS_H
#define FS_CRAMFS_H

#include <stdint.h>

typedef struct
{
    uint32_t fs_id;
    uint32_t block_size;
    uint32_t blocks;
    uint32_t flags;
} fs_cramfs_device_t;

typedef struct
{
    uint32_t inode;
    uint16_t mode;
    uint16_t uid;
    uint32_t size;
    uint32_t gid_offset;
} fs_cramfs_inode_t;

int fs_cramfs_init(void);
int fs_cramfs_mount(fs_cramfs_device_t *dev);
int fs_cramfs_read_inode(fs_cramfs_device_t *dev, uint32_t inode_num, fs_cramfs_inode_t *inode);
int fs_cramfs_read_block(fs_cramfs_device_t *dev, uint32_t block, uint8_t *buffer);
int fs_cramfs_get_file_size(fs_cramfs_device_t *dev, uint32_t inode, uint32_t *size);
int fs_cramfs_read_file(fs_cramfs_device_t *dev, uint32_t inode, uint8_t *buffer, uint32_t size);

#endif
