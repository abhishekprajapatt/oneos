#ifndef FS_NTFS_H
#define FS_NTFS_H

#include <stdint.h>

typedef struct
{
    uint32_t fs_id;
    uint32_t sector_size;
    uint32_t cluster_size;
    uint64_t volume_size;
    uint32_t flags;
} fs_ntfs_device_t;

typedef struct
{
    uint64_t mft_ref;
    uint16_t flags;
    uint32_t size;
    uint64_t mtime;
} fs_ntfs_inode_t;

int fs_ntfs_init(void);
int fs_ntfs_mount(fs_ntfs_device_t *dev);
int fs_ntfs_read_mft_record(fs_ntfs_device_t *dev, uint32_t record_num, uint8_t *buffer);
int fs_ntfs_read_inode(fs_ntfs_device_t *dev, fs_ntfs_inode_t *inode);
int fs_ntfs_read_file(fs_ntfs_device_t *dev, uint64_t mft_ref, uint8_t *buffer, uint32_t size);
int fs_ntfs_list_directory(fs_ntfs_device_t *dev, uint64_t mft_ref);

#endif
