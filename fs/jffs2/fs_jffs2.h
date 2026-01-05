#ifndef FS_JFFS2_H
#define FS_JFFS2_H

#include <stdint.h>

typedef struct
{
    uint32_t fs_id;
    uint32_t erase_block_size;
    uint32_t total_blocks;
    uint32_t flags;
} fs_jffs2_device_t;

typedef struct
{
    uint32_t inode;
    uint16_t mode;
    uint32_t size;
    uint32_t mtime;
} fs_jffs2_inode_t;

int fs_jffs2_init(void);
int fs_jffs2_mount(fs_jffs2_device_t *dev);
int fs_jffs2_scan_flash(fs_jffs2_device_t *dev);
int fs_jffs2_read_inode(fs_jffs2_device_t *dev, fs_jffs2_inode_t *inode);
int fs_jffs2_write_inode(fs_jffs2_device_t *dev, fs_jffs2_inode_t *inode);
int fs_jffs2_garbage_collect(fs_jffs2_device_t *dev);

#endif
