#ifndef FS_H
#define FS_H

#include <stdint.h>

typedef struct
{
    uint32_t block_size;
    uint32_t total_blocks;
    uint32_t free_blocks;
} fs_stat_t;

int fs_init(void);
int fs_mount(const char *device, const char *mountpoint);
int fs_umount(const char *mountpoint);
int fs_stat(const char *path, fs_stat_t *stat);
int fs_mkdir(const char *path);
int fs_rmdir(const char *path);
int fs_open(const char *path, int flags, void **fd);
int fs_close(void *fd);

#endif
