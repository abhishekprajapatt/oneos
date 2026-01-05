#ifndef ATA_DRIVER_H
#define ATA_DRIVER_H

#include <stdint.h>

#define ATA_CMD_READ_SECTORS 0x20
#define ATA_CMD_WRITE_SECTORS 0x30
#define ATA_CMD_IDENTIFY 0xEC

typedef struct
{
    uint16_t port_base;
    uint8_t device;
    uint32_t sector_count;
} ata_device_t;

typedef struct
{
    uint32_t lba;
    uint32_t count;
    void *buffer;
} ata_request_t;

int ata_init(void);
int ata_read_sectors(ata_device_t *dev, ata_request_t *req);
int ata_write_sectors(ata_device_t *dev, ata_request_t *req);
int ata_identify_device(ata_device_t *dev, void *buffer);

#endif
