#ifndef FS_CORE_H
#define FS_CORE_H

#include <stdint.h>

typedef struct
{
    uint64_t inode;
    uint32_t mode;
    uint64_t size;
    uint64_t ctime;
    uint64_t mtime;
} fs_core_stat_t;

typedef struct
{
    uint32_t fs_id;
    const char *name;
    uint32_t block_size;
    uint64_t total_blocks;
} fs_core_superblock_t;

int fs_core_init(void);
int fs_core_register_fs(fs_core_superblock_t *sb);
int fs_core_unregister_fs(uint32_t fs_id);
int fs_core_stat(uint32_t fs_id, uint64_t inode, fs_core_stat_t *stat);
int fs_core_create_inode(uint32_t fs_id, uint32_t mode);
int fs_core_delete_inode(uint32_t fs_id, uint64_t inode);

#endif
