#ifndef DMA_GENERIC_H
#define DMA_GENERIC_H

#include <stdint.h>

typedef struct
{
    uint32_t dma_id;
    uint32_t base_address;
    uint8_t channel_count;
    uint32_t max_burst;
} dma_generic_device_t;

typedef struct
{
    uint32_t src_addr;
    uint32_t dst_addr;
    uint32_t length;
    uint32_t flags;
} dma_generic_transfer_t;

int dma_generic_init(void);
int dma_generic_probe(dma_generic_device_t *dev);
int dma_generic_request_channel(dma_generic_device_t *dev, uint8_t *channel);
int dma_generic_release_channel(dma_generic_device_t *dev, uint8_t channel);
int dma_generic_prepare_transfer(dma_generic_device_t *dev, uint8_t channel, dma_generic_transfer_t *transfer);
int dma_generic_start_transfer(dma_generic_device_t *dev, uint8_t channel);

#endif
