#include "fs_iso9660.h"

int fs_iso9660_init(void)
{
    return 0;
}

int fs_iso9660_mount(fs_iso9660_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_iso9660_read_sector(fs_iso9660_device_t *dev, uint32_t sector, uint8_t *buffer)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}

int fs_iso9660_find_file(fs_iso9660_device_t *dev, const char *filename, fs_iso9660_dir_entry_t *entry)
{
    if (!dev || !filename || !entry)
    {
        return -1;
    }
    return 0;
}

int fs_iso9660_read_file(fs_iso9660_device_t *dev, fs_iso9660_dir_entry_t *entry, uint8_t *buffer, uint32_t size)
{
    if (!dev || !entry || !buffer)
    {
        return -1;
    }
    return 0;
}

int fs_iso9660_list_directory(fs_iso9660_device_t *dev, uint32_t extent)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
