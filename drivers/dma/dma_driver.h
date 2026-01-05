#ifndef DMA_DRIVER_H
#define DMA_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    uint32_t channel;
    uint32_t flags;
} dma_controller_t;

typedef struct
{
    uint32_t src_addr;
    uint32_t dst_addr;
    uint32_t length;
    uint32_t flags;
} dma_request_t;

int dma_init(void);
int dma_request_channel(dma_controller_t *ctrl, uint32_t *channel);
int dma_release_channel(dma_controller_t *ctrl, uint32_t channel);
int dma_transfer(dma_controller_t *ctrl, dma_request_t *req);
int dma_wait(dma_controller_t *ctrl, uint32_t channel);

#endif
