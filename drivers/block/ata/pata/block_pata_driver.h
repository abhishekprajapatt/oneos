#ifndef BLOCK_PATA_DRIVER_H
#define BLOCK_PATA_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint16_t iobase;
    uint16_t iobase_alt;
    uint32_t irq;
} block_pata_device_t;

typedef struct
{
    uint64_t lba;
    uint32_t count;
    void *buffer;
} block_pata_request_t;

int block_pata_init(void);
int block_pata_probe(void);
int block_pata_read(block_pata_device_t *dev, block_pata_request_t *req);
int block_pata_write(block_pata_device_t *dev, block_pata_request_t *req);
int block_pata_identify(block_pata_device_t *dev);
int block_pata_set_dma(block_pata_device_t *dev, uint32_t enable);

#endif
