#ifndef INTERRUPT_HANDLER_H
#define INTERRUPT_HANDLER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_INTERRUPTS 256
#define MAX_IRQ_HANDLERS 512

typedef enum
{
    IRQ_TIMER,
    IRQ_KEYBOARD,
    IRQ_MOUSE,
    IRQ_SERIAL,
    IRQ_PARALLEL,
    IRQ_DISK,
    IRQ_NETWORK,
    IRQ_AUDIO,
    IRQ_USB,
    IRQ_CUSTOM
} IRQType;

typedef struct
{
    uint32_t irq_number;
    IRQType type;
    bool enabled;
    uint32_t priority;
    uint32_t handler_count;
    void (**handlers)(uint32_t);
} InterruptDescriptor;

typedef struct
{
    uint32_t interrupt_count;
    uint32_t total_handled;
    time_t last_interrupt_time;
    InterruptDescriptor descriptors[MAX_INTERRUPTS];
} InterruptController;

InterruptController *irq_controller_init(void);
void irq_controller_cleanup(InterruptController *ic);
int irq_register_handler(InterruptController *ic, uint32_t irq_number, IRQType type,
                         void (*handler)(uint32_t), uint32_t priority);
int irq_unregister_handler(InterruptController *ic, uint32_t irq_number);
int irq_enable(InterruptController *ic, uint32_t irq_number);
int irq_disable(InterruptController *ic, uint32_t irq_number);
void irq_handle(InterruptController *ic, uint32_t irq_number);
void irq_acknowledge(InterruptController *ic, uint32_t irq_number);
uint32_t irq_get_handler_count(InterruptController *ic, uint32_t irq_number);
uint32_t irq_get_total_interrupts_handled(InterruptController *ic);

#endif
