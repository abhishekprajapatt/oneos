#include "media_cdrom.h"

int media_cdrom_init(void)
{
    return 0;
}

int media_cdrom_register_drive(media_cdrom_drive_t *drive)
{
    if (!drive)
    {
        return -1;
    }
    return 0;
}

int media_cdrom_unregister_drive(uint32_t drive_id)
{
    return 0;
}

int media_cdrom_mount(uint32_t drive_id)
{
    return 0;
}

int media_cdrom_unmount(uint32_t drive_id)
{
    return 0;
}

int media_cdrom_read_sector(uint32_t drive_id, uint32_t sector, uint8_t *buffer)
{
    if (!buffer)
    {
        return -1;
    }
    return 0;
}
