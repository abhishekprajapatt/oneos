#ifndef FS_SMB_H
#define FS_SMB_H

#include <stdint.h>

typedef struct
{
    uint32_t fs_id;
    uint8_t server_addr[4];
    uint16_t server_port;
    uint32_t flags;
} fs_smb_device_t;

typedef struct
{
    uint32_t fid;
    uint32_t mode;
    uint64_t size;
    uint64_t mtime;
} fs_smb_inode_t;

int fs_smb_init(void);
int fs_smb_connect(fs_smb_device_t *dev, const char *username, const char *password, const char *workgroup);
int fs_smb_disconnect(fs_smb_device_t *dev);
int fs_smb_open_file(fs_smb_device_t *dev, const char *path, uint32_t flags);
int fs_smb_read_file(fs_smb_device_t *dev, uint32_t fid, uint8_t *buffer, uint32_t size);
int fs_smb_write_file(fs_smb_device_t *dev, uint32_t fid, uint8_t *buffer, uint32_t size);

#endif
