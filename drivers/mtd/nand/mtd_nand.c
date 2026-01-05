#include "mtd_nand.h"

int mtd_nand_init(void)
{
    return 0;
}

int mtd_nand_probe(mtd_nand_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int mtd_nand_read_page(mtd_nand_device_t *dev, mtd_nand_page_t *page)
{
    if (!dev || !page)
    {
        return -1;
    }
    return 0;
}

int mtd_nand_write_page(mtd_nand_device_t *dev, mtd_nand_page_t *page)
{
    if (!dev || !page)
    {
        return -1;
    }
    return 0;
}

int mtd_nand_erase_block(mtd_nand_device_t *dev, uint32_t block)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int mtd_nand_bad_block_check(mtd_nand_device_t *dev, uint32_t block)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
