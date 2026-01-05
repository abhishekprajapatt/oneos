#include "dma_driver.h"

int dma_init(void)
{
    return 0;
}

int dma_request_channel(dma_controller_t *ctrl, uint32_t *channel)
{
    if (!ctrl || !channel)
    {
        return -1;
    }
    *channel = 0;
    return 0;
}

int dma_release_channel(dma_controller_t *ctrl, uint32_t channel)
{
    if (!ctrl)
    {
        return -1;
    }
    return 0;
}

int dma_transfer(dma_controller_t *ctrl, dma_request_t *req)
{
    if (!ctrl || !req)
    {
        return -1;
    }
    return 0;
}

int dma_wait(dma_controller_t *ctrl, uint32_t channel)
{
    if (!ctrl)
    {
        return -1;
    }
    return 0;
}
