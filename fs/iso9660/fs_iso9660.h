#ifndef FS_ISO9660_H
#define FS_ISO9660_H

#include <stdint.h>

typedef struct
{
    uint32_t fs_id;
    uint32_t sector_size;
    uint32_t path_table_location;
    uint32_t flags;
} fs_iso9660_device_t;

typedef struct
{
    uint32_t extent;
    uint32_t size;
    uint8_t name[128];
    uint8_t flags;
} fs_iso9660_dir_entry_t;

int fs_iso9660_init(void);
int fs_iso9660_mount(fs_iso9660_device_t *dev);
int fs_iso9660_read_sector(fs_iso9660_device_t *dev, uint32_t sector, uint8_t *buffer);
int fs_iso9660_find_file(fs_iso9660_device_t *dev, const char *filename, fs_iso9660_dir_entry_t *entry);
int fs_iso9660_read_file(fs_iso9660_device_t *dev, fs_iso9660_dir_entry_t *entry, uint8_t *buffer, uint32_t size);
int fs_iso9660_list_directory(fs_iso9660_device_t *dev, uint32_t extent);

#endif
