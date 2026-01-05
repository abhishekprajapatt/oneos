#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    const char *name;
    const char *class;
    uint32_t flags;
    void *private_data;
} device_t;

typedef struct
{
    uint32_t vendor_id;
    uint32_t product_id;
    uint32_t revision;
    uint32_t class_code;
} device_info_t;

#endif
