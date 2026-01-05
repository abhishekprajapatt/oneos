#ifndef FS_NFSV4_H
#define FS_NFSV4_H

#include <stdint.h>

typedef struct
{
    uint8_t server_addr[4];
    uint16_t server_port;
    uint32_t rpc_version;
    uint32_t flags;
} fs_nfsv4_device_t;

typedef struct
{
    uint64_t inode;
    uint32_t type;
    uint64_t size;
    uint64_t mtime;
} fs_nfsv4_inode_t;

int fs_nfsv4_init(void);
int fs_nfsv4_connect(fs_nfsv4_device_t *dev, const char *clientid);
int fs_nfsv4_disconnect(fs_nfsv4_device_t *dev);
int fs_nfsv4_lookup(fs_nfsv4_device_t *dev, const char *path);
int fs_nfsv4_read_file(fs_nfsv4_device_t *dev, uint64_t inode, uint64_t offset, uint8_t *buffer, uint32_t size);
int fs_nfsv4_write_file(fs_nfsv4_device_t *dev, uint64_t inode, uint64_t offset, uint8_t *buffer, uint32_t size);

#endif
