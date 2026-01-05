#ifndef STORAGE_MMC_H
#define STORAGE_MMC_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t base_address;
    uint8_t card_type;
    uint32_t block_count;
    uint32_t block_size;
} storage_mmc_device_t;

typedef struct
{
    uint32_t ocr;
    uint32_t cid[4];
    uint32_t csd[4];
    uint8_t state;
} storage_mmc_card_info_t;

int storage_mmc_init(void);
int storage_mmc_probe(storage_mmc_device_t *dev);
int storage_mmc_detect_card(storage_mmc_device_t *dev);
int storage_mmc_read_block(storage_mmc_device_t *dev, uint32_t block, uint8_t *buffer);
int storage_mmc_write_block(storage_mmc_device_t *dev, uint32_t block, uint8_t *buffer);
int storage_mmc_erase_blocks(storage_mmc_device_t *dev, uint32_t start, uint32_t count);

#endif
