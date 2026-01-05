#ifndef BUS_H
#define BUS_H

#include <stdint.h>

typedef struct
{
    uint32_t bus_id;
    const char *name;
    uint32_t bus_type;
    uint32_t device_count;
} bus_t;

#define BUS_TYPE_PCI 1
#define BUS_TYPE_USB 2
#define BUS_TYPE_I2C 3
#define BUS_TYPE_SPI 4
#define BUS_TYPE_PLATFORM 5

#endif
