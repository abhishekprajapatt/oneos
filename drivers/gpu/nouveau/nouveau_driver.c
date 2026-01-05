#include "nouveau_driver.h"

int nouveau_init(void)
{
    return 0;
}

int nouveau_probe(void)
{
    return 0;
}

int nouveau_open(nouveau_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int nouveau_close(nouveau_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int nouveau_channel_alloc(nouveau_device_t *dev, nouveau_channel_t *chan)
{
    if (!dev || !chan)
    {
        return -1;
    }
    return 0;
}

int nouveau_channel_free(nouveau_device_t *dev, nouveau_channel_t *chan)
{
    if (!dev || !chan)
    {
        return -1;
    }
    return 0;
}

int nouveau_pushbuf_push(nouveau_channel_t *chan, uint32_t *data, uint32_t len)
{
    if (!chan || !data)
    {
        return -1;
    }
    return 0;
}
