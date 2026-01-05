#ifndef CLK_H
#define CLK_H

#include <stdint.h>

typedef struct
{
    uint32_t clock_id;
    const char *name;
    uint32_t rate;
    uint32_t parent_id;
    uint32_t flags;
} clock_t;

#define CLK_ENABLE 0x01
#define CLK_DISABLE 0x02
#define CLK_CRITICAL 0x04

#endif
