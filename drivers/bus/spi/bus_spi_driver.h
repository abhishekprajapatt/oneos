#ifndef BUS_SPI_DRIVER_H
#define BUS_SPI_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t bus_id;
    uint8_t chip_select;
    uint32_t speed;
    uint8_t mode;
} bus_spi_device_t;

typedef struct
{
    uint8_t *tx_buf;
    uint8_t *rx_buf;
    uint32_t len;
    uint32_t speed;
    uint8_t bits_per_word;
} bus_spi_transfer_t;

int bus_spi_init(void);
int bus_spi_register_master(uint32_t bus_id, uint32_t num_devices);
int bus_spi_register_device(bus_spi_device_t *dev);
int bus_spi_transfer(bus_spi_device_t *dev, bus_spi_transfer_t *xfer);
int bus_spi_write(bus_spi_device_t *dev, uint8_t *data, uint32_t len);
int bus_spi_read(bus_spi_device_t *dev, uint8_t *data, uint32_t len);

#endif
