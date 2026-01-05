#ifndef FS_CIFS_H
#define FS_CIFS_H

#include <stdint.h>

typedef struct
{
    uint32_t fs_id;
    uint8_t server_addr[4];
    uint16_t server_port;
    uint32_t flags;
} fs_cifs_device_t;

typedef struct
{
    uint64_t inode_num;
    uint32_t mode;
    uint64_t size;
    uint64_t mtime;
} fs_cifs_inode_t;

int fs_cifs_init(void);
int fs_cifs_connect(fs_cifs_device_t *dev, const char *username, const char *password);
int fs_cifs_disconnect(fs_cifs_device_t *dev);
int fs_cifs_open_file(fs_cifs_device_t *dev, const char *path, uint32_t flags);
int fs_cifs_read_file(fs_cifs_device_t *dev, uint32_t file_id, uint8_t *buffer, uint32_t size);
int fs_cifs_write_file(fs_cifs_device_t *dev, uint32_t file_id, uint8_t *buffer, uint32_t size);

#endif
