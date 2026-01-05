#ifndef BLOCK_SATA_DRIVER_H
#define BLOCK_SATA_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t phy_id;
    uint32_t port_num;
    uint32_t flags;
} block_sata_device_t;

typedef struct
{
    uint64_t lba;
    uint32_t count;
    void *buffer;
    uint32_t flags;
} block_sata_request_t;

int block_sata_init(void);
int block_sata_probe(void);
int block_sata_phy_up(block_sata_device_t *dev);
int block_sata_read(block_sata_device_t *dev, block_sata_request_t *req);
int block_sata_write(block_sata_device_t *dev, block_sata_request_t *req);
int block_sata_flush(block_sata_device_t *dev);

#endif
