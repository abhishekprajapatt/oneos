#ifndef MTD_NAND_H
#define MTD_NAND_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t base_address;
    uint32_t block_count;
    uint32_t page_size;
    uint32_t block_size;
} mtd_nand_device_t;

typedef struct
{
    uint32_t page;
    uint8_t *data;
    uint8_t *oob;
} mtd_nand_page_t;

int mtd_nand_init(void);
int mtd_nand_probe(mtd_nand_device_t *dev);
int mtd_nand_read_page(mtd_nand_device_t *dev, mtd_nand_page_t *page);
int mtd_nand_write_page(mtd_nand_device_t *dev, mtd_nand_page_t *page);
int mtd_nand_erase_block(mtd_nand_device_t *dev, uint32_t block);
int mtd_nand_bad_block_check(mtd_nand_device_t *dev, uint32_t block);

#endif
