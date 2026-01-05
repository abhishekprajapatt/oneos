#include "fs_core.h"

int fs_core_init(void)
{
    return 0;
}

int fs_core_register_fs(fs_core_superblock_t *sb)
{
    if (!sb)
    {
        return -1;
    }
    return 0;
}

int fs_core_unregister_fs(uint32_t fs_id)
{
    return 0;
}

int fs_core_stat(uint32_t fs_id, uint64_t inode, fs_core_stat_t *stat)
{
    if (!stat)
    {
        return -1;
    }
    return 0;
}

int fs_core_create_inode(uint32_t fs_id, uint32_t mode)
{
    return 0;
}

int fs_core_delete_inode(uint32_t fs_id, uint64_t inode)
{
    return 0;
}
