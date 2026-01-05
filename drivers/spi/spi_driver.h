#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t bus_id;
    uint32_t speed;
    uint8_t mode;
    uint8_t bits_per_word;
} spi_master_t;

typedef struct
{
    uint8_t *tx_buf;
    uint8_t *rx_buf;
    uint32_t len;
    uint32_t flags;
} spi_message_t;

int spi_init(void);
int spi_register_master(spi_master_t *master);
int spi_transfer(spi_master_t *master, spi_message_t *msg);
int spi_write(spi_master_t *master, uint8_t *data, uint32_t len);
int spi_read(spi_master_t *master, uint8_t *data, uint32_t len);

#endif
