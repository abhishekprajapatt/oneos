#ifndef FS_SQUASHFS_H
#define FS_SQUASHFS_H

#include <stdint.h>

typedef struct
{
    uint32_t fs_id;
    uint32_t block_size;
    uint32_t blocks;
    uint32_t compression_type;
    uint32_t flags;
} fs_squashfs_device_t;

typedef struct
{
    uint32_t inode;
    uint16_t mode;
    uint32_t size;
    uint32_t mtime;
} fs_squashfs_inode_t;

int fs_squashfs_init(void);
int fs_squashfs_mount(fs_squashfs_device_t *dev);
int fs_squashfs_read_inode(fs_squashfs_device_t *dev, uint32_t inode_num, fs_squashfs_inode_t *inode);
int fs_squashfs_read_block(fs_squashfs_device_t *dev, uint32_t block, uint8_t *buffer);
int fs_squashfs_decompress_block(fs_squashfs_device_t *dev, uint8_t *compressed, uint8_t *decompressed);
int fs_squashfs_read_file(fs_squashfs_device_t *dev, uint32_t inode, uint8_t *buffer, uint32_t size);

#endif
