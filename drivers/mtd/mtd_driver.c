#include "mtd_driver.h"

int mtd_init(void)
{
    return 0;
}

int mtd_register(mtd_info_t *info)
{
    if (!info)
    {
        return -1;
    }
    return 0;
}

int mtd_read(mtd_info_t *info, mtd_request_t *req)
{
    if (!info || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int mtd_write(mtd_info_t *info, mtd_request_t *req)
{
    if (!info || !req || !req->buffer)
    {
        return -1;
    }
    return 0;
}

int mtd_erase(mtd_info_t *info, uint64_t addr, uint32_t len)
{
    if (!info)
    {
        return -1;
    }
    return 0;
}
