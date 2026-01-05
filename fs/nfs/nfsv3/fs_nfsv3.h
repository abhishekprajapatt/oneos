#ifndef FS_NFSV3_H
#define FS_NFSV3_H

#include <stdint.h>

typedef struct
{
    uint8_t server_addr[4];
    uint16_t server_port;
    uint32_t rpc_version;
    uint32_t flags;
} fs_nfsv3_device_t;

typedef struct
{
    uint64_t inode;
    uint32_t mode;
    uint64_t size;
    uint32_t mtime_sec;
    uint32_t mtime_nsec;
} fs_nfsv3_inode_t;

int fs_nfsv3_init(void);
int fs_nfsv3_connect(fs_nfsv3_device_t *dev);
int fs_nfsv3_disconnect(fs_nfsv3_device_t *dev);
int fs_nfsv3_lookup(fs_nfsv3_device_t *dev, const char *path);
int fs_nfsv3_read_file(fs_nfsv3_device_t *dev, uint64_t inode, uint64_t offset, uint8_t *buffer, uint32_t size);
int fs_nfsv3_write_file(fs_nfsv3_device_t *dev, uint64_t inode, uint64_t offset, uint8_t *buffer, uint32_t size);

#endif
