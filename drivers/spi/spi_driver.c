#include "spi_driver.h"

int spi_init(void)
{
    return 0;
}

int spi_register_master(spi_master_t *master)
{
    if (!master)
    {
        return -1;
    }
    return 0;
}

int spi_transfer(spi_master_t *master, spi_message_t *msg)
{
    if (!master || !msg)
    {
        return -1;
    }
    return 0;
}

int spi_write(spi_master_t *master, uint8_t *data, uint32_t len)
{
    if (!master || !data || len == 0)
    {
        return -1;
    }
    return 0;
}

int spi_read(spi_master_t *master, uint8_t *data, uint32_t len)
{
    if (!master || !data || len == 0)
    {
        return -1;
    }
    return 0;
}
