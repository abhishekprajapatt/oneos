#ifndef STORAGE_DRIVER_H
#define STORAGE_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint64_t capacity;
    uint32_t sector_size;
} storage_device_t;

typedef struct
{
    uint64_t sector;
    uint32_t count;
    void *buffer;
} storage_request_t;

int storage_init(void);
int storage_detect(void);
int storage_read(storage_device_t *dev, storage_request_t *req);
int storage_write(storage_device_t *dev, storage_request_t *req);
int storage_flush(storage_device_t *dev);

#endif
