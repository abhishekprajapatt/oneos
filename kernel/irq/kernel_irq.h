#ifndef KERNEL_IRQ_H
#define KERNEL_IRQ_H

#include <stdint.h>

typedef struct
{
    uint32_t irq_num;
    void (*handler)(uint32_t);
    void *dev_id;
    uint32_t flags;
} kernel_irq_handler_t;

typedef struct
{
    uint32_t irq_count;
    uint64_t total_interrupts;
    uint32_t pending_count;
} kernel_irq_state_t;

int kernel_irq_init(void);
int kernel_irq_register_handler(kernel_irq_handler_t *handler);
int kernel_irq_unregister_handler(uint32_t irq_num, void *dev_id);
int kernel_irq_enable(uint32_t irq_num);
int kernel_irq_disable(uint32_t irq_num);
int kernel_irq_trigger(uint32_t irq_num);

#endif
