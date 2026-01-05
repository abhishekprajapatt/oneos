#ifndef DMA_H
#define DMA_H

#include <stdint.h>

typedef struct
{
    uint32_t dma_channel;
    uint32_t source_addr;
    uint32_t dest_addr;
    uint32_t transfer_size;
    uint32_t flags;
} dma_transfer_t;

#define DMA_TRANSFER_MEMORY 0x01
#define DMA_TRANSFER_PERIPH 0x02
#define DMA_TRANSFER_SINGLE 0x04
#define DMA_TRANSFER_BURST 0x08

#endif
