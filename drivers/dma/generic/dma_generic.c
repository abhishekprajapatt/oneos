#include "dma_generic.h"

int dma_generic_init(void)
{
    return 0;
}

int dma_generic_probe(dma_generic_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int dma_generic_request_channel(dma_generic_device_t *dev, uint8_t *channel)
{
    if (!dev || !channel)
    {
        return -1;
    }
    return 0;
}

int dma_generic_release_channel(dma_generic_device_t *dev, uint8_t channel)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int dma_generic_prepare_transfer(dma_generic_device_t *dev, uint8_t channel, dma_generic_transfer_t *transfer)
{
    if (!dev || !transfer)
    {
        return -1;
    }
    return 0;
}

int dma_generic_start_transfer(dma_generic_device_t *dev, uint8_t channel)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
