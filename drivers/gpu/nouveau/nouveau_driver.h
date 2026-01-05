#ifndef NOUVEAU_DRIVER_H
#define NOUVEAU_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t chip;
    uint32_t vram_size;
} nouveau_device_t;

typedef struct
{
    uint32_t channel_id;
    uint32_t pushbuf;
    uint32_t flags;
} nouveau_channel_t;

int nouveau_init(void);
int nouveau_probe(void);
int nouveau_open(nouveau_device_t *dev);
int nouveau_close(nouveau_device_t *dev);
int nouveau_channel_alloc(nouveau_device_t *dev, nouveau_channel_t *chan);
int nouveau_channel_free(nouveau_device_t *dev, nouveau_channel_t *chan);
int nouveau_pushbuf_push(nouveau_channel_t *chan, uint32_t *data, uint32_t len);

#endif
