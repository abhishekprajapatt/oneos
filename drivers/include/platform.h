#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

typedef struct
{
    uint32_t resource_id;
    uint32_t type;
    uint32_t start;
    uint32_t end;
    const char *name;
} platform_resource_t;

#define IORESOURCE_MEM 0x00000200
#define IORESOURCE_IRQ 0x00000400
#define IORESOURCE_DMA 0x00000800

#endif
