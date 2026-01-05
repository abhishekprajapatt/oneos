#ifndef STORAGE_ATA_H
#define STORAGE_ATA_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t base_address;
    uint8_t channel;
    uint32_t flags;
    uint32_t sector_count;
} storage_ata_device_t;

typedef struct
{
    uint8_t status;
    uint8_t error;
    uint16_t sector_count;
    uint32_t lba;
} storage_ata_status_t;

int storage_ata_init(void);
int storage_ata_probe(storage_ata_device_t *dev);
int storage_ata_identify(storage_ata_device_t *dev);
int storage_ata_read_sector(storage_ata_device_t *dev, uint32_t lba, uint8_t *buffer, uint32_t count);
int storage_ata_write_sector(storage_ata_device_t *dev, uint32_t lba, uint8_t *buffer, uint32_t count);
int storage_ata_set_power_mode(storage_ata_device_t *dev, uint8_t mode);

#endif
