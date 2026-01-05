#ifndef MEDIA_CDROM_H
#define MEDIA_CDROM_H

#include <stdint.h>

typedef struct
{
    uint32_t drive_id;
    const char *drive_name;
    uint32_t media_present;
    uint64_t media_capacity;
} media_cdrom_drive_t;

typedef struct
{
    uint32_t drive_count;
    uint32_t mounted_count;
    uint64_t total_capacity;
} media_cdrom_state_t;

int media_cdrom_init(void);
int media_cdrom_register_drive(media_cdrom_drive_t *drive);
int media_cdrom_unregister_drive(uint32_t drive_id);
int media_cdrom_mount(uint32_t drive_id);
int media_cdrom_unmount(uint32_t drive_id);
int media_cdrom_read_sector(uint32_t drive_id, uint32_t sector, uint8_t *buffer);

#endif
