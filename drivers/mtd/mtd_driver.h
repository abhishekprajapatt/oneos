#ifndef MTD_DRIVER_H
#define MTD_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    uint64_t size;
    uint32_t erasesize;
    uint32_t writesize;
    uint32_t oobsize;
} mtd_info_t;

typedef struct
{
    uint64_t offset;
    uint32_t length;
    void *buffer;
} mtd_request_t;

int mtd_init(void);
int mtd_register(mtd_info_t *info);
int mtd_read(mtd_info_t *info, mtd_request_t *req);
int mtd_write(mtd_info_t *info, mtd_request_t *req);
int mtd_erase(mtd_info_t *info, uint64_t addr, uint32_t len);

#endif
