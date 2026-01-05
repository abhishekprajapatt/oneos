#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

typedef int (*irq_handler_t)(uint32_t irq, void *dev);

typedef struct
{
    uint32_t irq_number;
    const char *name;
    irq_handler_t handler;
    uint32_t flags;
} irq_descriptor_t;

#define IRQ_TYPE_LEVEL_HIGH 0
#define IRQ_TYPE_LEVEL_LOW 1
#define IRQ_TYPE_EDGE_RISING 2
#define IRQ_TYPE_EDGE_FALLING 3

#endif
